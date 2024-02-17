#include "Arduino.h"
#include "micronova_stove.h"

#if defined(ESP8266) || defined(USE_SWSERIAL)
  //#include <TasmotaSerial.h>
  TasmotaSerial StoveSerial(STOVE_PIN_RX, STOVE_PIN_TX);
#endif

#if defined(ESP32) || defined(USE_HWSERIAL)
  #include <HardwareSerial.h>
  HardwareSerial StoveSerial(1);
#endif

#ifdef USE_RX_ENABLE
    MicronovaStove::MicronovaStove( int rx, int tx, int rx_enable ) {
        pin_rx = rx;
        pin_tx = tx;
        pin_rx_enable = rx_enable;
    }
#else
    MicronovaStove::MicronovaStove( int rx, int tx ) {
        pin_rx = rx;
        pin_tx = tx;
    }
#endif

bool MicronovaStove::init(){
  //StoveSerial.begin( 1200, SERIAL_8N2, STOVE_PIN_RX, STOVE_PIN_TX, false );
  StoveSerial.begin( 1200, SERIAL_8N2);
  
  if (dbg_out) printf("begin stove serial");
#ifdef USE_RX_ENABLE
    pinMode( pin_rx_enable, OUTPUT );
    digitalWrite( pin_rx_enable, HIGH ); 
#endif
  return StoveSerial.isValid();
}

/* read functions */
#ifdef USE_RX_ENABLE
void MicronovaStove::enable_rx(){
  if (dbg_out) printf("enable rx\n");
  digitalWrite( pin_rx_enable, LOW );
}

void MicronovaStove::disable_rx(){
  if (dbg_out) printf("disable rx\n");
  digitalWrite( pin_rx_enable, HIGH );
}
#endif

void MicronovaStove::flushInput(){
  if (dbg_out) printf("flush serial input\n");
  while (StoveSerial.available()){
    StoveSerial.read();
    if (dbg_out) printf("flush: read 1 extra byte\n");
  }
}

void MicronovaStove::read(uint8_t location, uint8_t addr){
      
  dbg_out=false;

  if (dbg_out){
    printf("read request loc=");
    printf("0x%02x ", location );
    printf(", addr=");
    printf("0x%02x ", addr );
    printf("\n");
  }

  StoveSerial.write(location);
  StoveSerial.flush();
  StoveSerial.write(addr);
  StoveSerial.flush();
#ifdef USE_RX_ENABLE
  enable_rx();
#endif
  // the stove needs some time to answer, 120ms seems about the right amount, 110 is too short
  //delay(120);
  // MODIFICATO DA 80 (funzionante) a 120 (da testare)
  delay(120);

  if (dbg_out) printf("read stove answer\n");

  uint8_t rx_count = 0;
  stove_rx_data[0] = 0x00;
  stove_rx_data[1] = 0x00;
      
  while ( StoveSerial.available() ) {
    if (rx_count>1){
      // TODO: find a better way to do this
      rx_count = 0;
    }
    stove_rx_data[rx_count] = StoveSerial.read();
    if (dbg_out) printf("i=0x%02x data=0x%02x\n",rx_count,stove_rx_data[rx_count]);
    rx_count++;
  }

  #ifdef USE_RX_ENABLE    
  disable_rx();
  #endif
  last_read_value = stove_rx_data[1];
  last_read_checksum = stove_rx_data[0];
  last_read_param = last_read_checksum - last_read_value;

  if (last_read_param != addr ){
    if (dbg_out) printf("E: return mismatch\n");
  }
  // TODO: check for error with checksum
}

uint8_t MicronovaStove::read_ram( uint8_t addr ){
  read( STOVE_OFFSET_RAM_READ, addr );
  return last_read_value;
}

uint8_t MicronovaStove::read_eeprom( uint8_t addr ){
  read( STOVE_OFFSET_EEPROM_READ, addr );
  return last_read_value;
}

int8_t MicronovaStove::read_and_store_topics(){
  int8_t ret = -1;

  RWrunning = true;

  // READ FUNCTION - begin
  StoveSerial.write(STOVE_OFFSET_RAM_READ);
  StoveSerial.flush();
  StoveSerial.write(topics[topicID].address);
  StoveSerial.flush();
#ifdef USE_RX_ENABLE
  enable_rx();
#endif
  delay(120);
  uint8_t rx_count = 0;
  stove_rx_data[0] = 0x00;
  stove_rx_data[1] = 0x00;
      
  while ( StoveSerial.available() ) {
    if (rx_count>1){
      // TODO: find a better way to do this
      rx_count = 0;
    }
    stove_rx_data[rx_count] = StoveSerial.read();
    rx_count++;
  }
#ifdef USE_RX_ENABLE    
  disable_rx();
#endif

  // READ FUNCTION - end

  if (stove_rx_data[0] - stove_rx_data[1] == topics[topicID].address) {
    topics[topicID].newValue = (float) stove_rx_data[1];
    topics[topicID].newValue *= topics[topicID].factor;
    if (topics[topicID].newValue != topics[topicID].currentValue) {
      topics[topicID].currentValue = topics[topicID].newValue;
      // DBG
      //topics[topicID].currentValue = topics[topicID].address;
      ret = topicID;
    }
  } else {
    // ERRORE SERIALE
  }
    
  // topics[topicID].newValue = (float) read_ram(topics[topicID].address);
  // topics[topicID].newValue *= topics[topicID].factor;
  // if (topics[topicID].newValue != topics[topicID].currentValue) {
  //   topics[topicID].currentValue = topics[topicID].newValue;
  //   topics[topicID].currentValue = topics[topicID].address;
  //   ret = &topics[topicID];
  // }

  if (topicID++ >= N_TOPICS) topicID = 0;

  RWrunning = false;
  
  return ret;
}

unsigned long MicronovaStove::measurePellet(){
  digitalWrite( PELLET_TRIG, HIGH );
  delayMicroseconds(15);
  digitalWrite( PELLET_TRIG, LOW );
  pelletMeasure = pulseIn(PELLET_ECHO, HIGH, 10000);
  pelletMeasure = pelletMeasure / 58;
  // TODO: send MQTT message
  return pelletMeasure;
}

/* write functions */

void MicronovaStove::write( uint8_t location, uint8_t command, uint8_t data ){
  uint8_t chk = calculate_checksum( location, command, data );
  uint8_t data_to_write[4] = {
    location,
    command,
    data,
    chk
  };
  
  if (dbg_out) printf("write data=");
    
  for ( int i = 0; i < 4; i++ ){
    if (dbg_out) printf("0x%02x ", data_to_write[i] );
    StoveSerial.write( data_to_write[i] );
    delay(10);
  }
    
  if (dbg_out) printf("\n");
}

void MicronovaStove::read_answer(){
  #ifdef USE_RX_ENABLE
  enable_rx();
  #endif
  //delay(120);
  delay(80);
  uint8_t rx_count = 0;
  stove_rx_data[0] = 0x00;
  stove_rx_data[1] = 0x00;
      
  while ( StoveSerial.available() ) {
    if (rx_count>1){
      // TODO: find a better way to do this
      rx_count = 0;
    }
    stove_rx_data[rx_count] = StoveSerial.read();
    rx_count++;
  }
  #ifdef USE_RX_ENABLE
  disable_rx();
  #endif
  last_read_value = stove_rx_data[1];
  last_read_checksum = stove_rx_data[0];
  
}

void MicronovaStove::write_ram( uint8_t command, uint8_t data ){
  write( STOVE_OFFSET_RAM_WRITE, command, data );
}

void MicronovaStove::write_eeprom( uint8_t command, uint8_t data ){
  write( STOVE_OFFSET_EEPROM_WRITE, command, data);
}

void MicronovaStove::simulate_infrared(uint8_t data, uint8_t repetitions){
  for (uint8_t i=0; i<repetitions; i++){
    write_ram(STOVE_ADDR_IRCOMMAND, data);
    delay(100);
  }
}

byte MicronovaStove::calculate_checksum( uint8_t dest, uint8_t addr, uint8_t val ){
  uint8_t checksum = 0;
  checksum = dest+addr+val;
  if ( checksum >= 256 ){
    checksum = checksum - 256;
  }
  return (uint8_t)checksum;
}

/* Abstraction helper functions */

void MicronovaStove::on(){
  write_ram(STOVE_ADDR_STATE, STOVE_STATE_TURN_ON);
}

void MicronovaStove::powerIR(){
  simulate_infrared(STOVE_IR_POWER, 10);
}

void MicronovaStove::off(){
  write_ram(STOVE_ADDR_STATE, STOVE_STATE_TURN_OFF);
}

uint8_t MicronovaStove::get_status(){
  read_ram(STOVE_ADDR_STATE);
  return last_read_value;
}

float MicronovaStove::get_ambient_temp(){
  read_ram(STOVE_ADDR_AMBIENT_TEMP);
  return (float) last_read_value / 2;
}

uint8_t MicronovaStove::get_fumes_temp(){
  read_ram(STOVE_ADDR_FUMES_TEMP);
  return last_read_value;
}

uint8_t MicronovaStove::get_water_temp(){
  read_ram(STOVE_ADDR_WATER_TEMP);
  return last_read_value;
}

uint8_t MicronovaStove::get_power(){
  read_ram(STOVE_ADDR_POWER_EEPROM);
  return last_read_value;
}

uint16_t MicronovaStove::get_fumes_fan_speed(){
  read_ram(STOVE_ADDR_FUMES_SPEED);
  return (uint16_t) last_read_value*10;
}

void MicronovaStove::set_power(uint8_t power_level){
  if (power_level > 4){
    power_level = 4;
  }
  // TODO: check if we really need to do this twice
  write_eeprom(STOVE_ADDR_POWER_EEPROM, power_level);
  write_ram(STOVE_ADDR_POWER_RAM, power_level);
}

void MicronovaStove::set_thermostat(uint8_t temperature){
  if (temperature > 32){
    temperature = 32;
  }
  write_eeprom(STOVE_ADDR_THERMOSTAT, temperature);
}