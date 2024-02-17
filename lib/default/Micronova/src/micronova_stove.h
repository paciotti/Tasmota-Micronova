#include <Arduino.h>

#ifndef _MICRONOVA_STOVE_H
  #define _MICRONOVA_STOVE_H

// TODO: Find the right places for those defines
// TODO: Ideally pin numbers should be selected by template

#define USE_RX_ENABLE
//#undef USE_RX_ENABLE

#define STOVE_PIN_RX        4
#define STOVE_PIN_TX        5
#ifdef USE_RX_ENABLE
  #define STOVE_PIN_EN_RX   2
#endif

#define PELLET_TRIG 14
#define PELLET_ECHO 16

#if defined(ESP8266) || defined(USE_SWSERIAL)
  #include <TasmotaSerial.h>
  extern TasmotaSerial StoveSerial;
  #define SERIAL_MODE SWSERIAL_8N2 
#endif

#if defined(ESP32) || defined(USE_HWSERIAL)
  #include <HardwareSerial.h>
  extern HardwareSerial StoveSerial;
  #define SERIAL_MODE SERIAL_8N2 
#endif

#define STOVE_OFFSET_RAM_READ     0x00
#define STOVE_OFFSET_EEPROM_READ  0x20
#define STOVE_OFFSET_RAM_WRITE    0x80
#define STOVE_OFFSET_EEPROM_WRITE 0xA0

// addresses
#define STOVE_ADDR_AMBIENT_TEMP   0x01
#define STOVE_ADDR_WATER_TEMP     0x03
#define STOVE_ADDR_PELLET_TIME    0x0D
#define STOVE_ADDR_STATE          0x21
#define STOVE_ADDR_FUMES_TEMP     0x3E
#define STOVE_ADDR_FUMES_SPEED    0x37
#define STOVE_ADDR_POWER_RAM      0x34
#define STOVE_ADDR_POWER_EEPROM   0x7F
#define STOVE_ADDR_IRCOMMAND      0x58
#define STOVE_ADDR_THERMOSTAT     0x7D

// STATE
#define STOVE_STATE_TURN_ON       0x01
#define STOVE_STATE_TURN_OFF      0x06
#define STOVE_STATE_FORCE_OFF     0x00

// IRCOMMANDS
#define STOVE_IR_POWERUP          0x54
#define STOVE_IR_POWERDOWN        0x50
#define STOVE_IR_TEMPUP           0x52
#define STOVE_IR_TEMPDOWN         0x58
#define STOVE_IR_POWER            0x5A

//0 - OFF, 1 - Starting, 2 - Pellet loading, 3 - Ignition, 4 - Work, 5 - Brazier cleaning, 6 - Final cleaning, 7 - Standby, 8 - Pellet missing alarm, 9 - Ignition failure alarm, 10 - Alarms (to be investigated)

struct stoveInfo{
  int address;
  float newValue;
  float currentValue;
  char topic[20];
  char unit[5];
  float factor;
};

class MicronovaStove {

  private:

    int pin_rx;
    int pin_tx;
#ifdef USE_RX_ENABLE
    int pin_rx_enable;
#endif

    // char stoveStatus[11] = {
    //   "Off",                      
    //   "Start",
    //   "Pellet loading",
    //   "Ignition",
    //   "Work",
    //   "Brazier cleaning",
    //   "Final cleaning",
    //   "Standby",
    //   "Pellet missing alarm",
    //   "Ignition failure alarm",
    //   "Undefined alarms"
    // };

  public:

    // [ADDRESS, NewValue, CurrentValue, Topic, Unit, Multiplier]
    // Commented line excluded from reading/publishing
    #define N_TOPICS 4
    stoveInfo topics[N_TOPICS] = {
       {STOVE_ADDR_WATER_TEMP,0,-1,"Water Temperature","C",1}
      ,{STOVE_ADDR_PELLET_TIME,0,-1,"Pellet Loading Time","sec",10}
      ,{STOVE_ADDR_STATE,0,-1,"Status"," ",1}
      ,{STOVE_ADDR_POWER_RAM,0,-1,"Current Power","#",1}
      //,{STOVE_ADDR_FUMES_SPEED,0,-1,"Smoke Fan Speed",    "rpm",10}
      //,{0x5A,0,-1,"Gas Temperature",    "C",  1}
      //,{0x00,0,-1,"Internal CPU",       "#",  1}
      //,{STOVE_ADDR_AMBIENT_TEMP,0,-1,"Room Temperature",   "C",  0.5}
    };
    int topicID = 0;

    bool dbg_out = true;
    char stove_rx_data[2];
    uint8_t last_read_value;
    uint8_t last_read_param;
    uint8_t last_read_checksum;
    unsigned long pelletMeasure;
    bool RWrunning = false;

#ifdef USE_RX_ENABLE
    MicronovaStove( int rx, int tx, int rx_enable );
#else
    MicronovaStove( int rx, int tx );
#endif

    bool init();

    /* read functions */
#ifdef USE_RX_ENABLE
    void enable_rx();
    void disable_rx();
#endif
    void flushInput();
    void read(uint8_t location, uint8_t addr);
    uint8_t read_ram( uint8_t addr );
    uint8_t read_eeprom( uint8_t addr );
    int8_t read_and_store_topics();
    unsigned long measurePellet();

    /* write functions */
    void write( uint8_t location, uint8_t command, uint8_t data );
    void write_ram( uint8_t command, uint8_t data );
    void write_eeprom( uint8_t command, uint8_t data );
    void simulate_infrared(uint8_t data, uint8_t repetitions );
    byte calculate_checksum( uint8_t dest, uint8_t addr, uint8_t val );
    void read_answer();

    /* misc abstraction functions */
    void on();
    void powerIR();
    void off();
    uint8_t get_status();
    float get_ambient_temp();
    uint8_t get_fumes_temp();
    uint8_t get_water_temp();
    uint8_t get_power();
    uint16_t get_fumes_fan_speed();
    void set_power(uint8_t power_level);
    void set_thermostat(uint8_t temperature);
};

#endif