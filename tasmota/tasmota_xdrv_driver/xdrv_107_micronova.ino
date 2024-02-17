#define USE_MICRONOVA
#ifdef USE_MICRONOVA

#include <micronova_stove.h>
#include "string.h"
// #include <PubSubClient.h>
// #include <WiFiClient.h>

#warning **** Micronova Driver is included... ****

#define XDRV_107 107

#define MICRONOVA_TX 2
#define MICRONOVA_RX 4

#define PELLET_TRIG 14
#define PELLET_ECHO 16
#define PELLET_MEASURE_PERIOD 60 // seconds

#define SEND_MQTT_PERIOD 15 // seconds, only for debug
int mqttDelay = 0;
// IPAddress server(192, 168, 1, 90);
// PubSubClient client;
// WiFiClient wifi;
/*********************************************************************************************\
 * Micronova Functions
\*********************************************************************************************/

// This variable will be set to true after initialization
bool initSuccess = false;

int pelletMeasureDelay = 0;

int8_t stoveResult;
char * payload = nullptr;
size_t payload_size = 20;
char * topic = nullptr;
size_t topic_size = 20;

unsigned long pellet_echo_lenght, pellet_cm;

#ifdef USE_RX_ENABLE
MicronovaStove stove = MicronovaStove(
    STOVE_PIN_RX,
    STOVE_PIN_TX,
    STOVE_PIN_EN_RX
);
#else
MicronovaStove stove = MicronovaStove(
    Pin(GPIO_RXD),
    Pin(GPIO_TXD)
);
#endif

//TasmotaSerial *serialTest;//(MICRONOVA_RX, MICRONOVA_TX);

/* 
  Optional: if you need to pass any command for your device 
  Commands are issued in Console or Web Console
  Commands:
    TogglePolling  - Only prints some text. Can be made something more useful...
    SendMQTT   - Send a MQTT example message
    Help       - Prints a list of available commands  
*/

const char MicronovaCommands[] PROGMEM = "|"  // No Prefix
  "MeasurePellet|" 
  "SendMQTT|"
  "Help|"
  "dbg|"
  "StoveOn|"
  "StoveOff|"
  "StoveIR|"
  "readStove";

void (* const MicronovaCommand[])(void) PROGMEM = {
  &CmdMeasurePellet, &CmdSendMQTT, &CmdHelp, &CmdDbg, &CmdStoveOn, &CmdStoveOff, &CmdStoveIR, &CmdReadStove };

void CmdReadStove(void){
  int st = stove.get_status();
  float temp = stove.get_ambient_temp();
  int water = stove.get_water_temp();
  AddLog(LOG_LEVEL_INFO, "Stove status is %d", st);
  AddLog(LOG_LEVEL_INFO, "Stove ambient temp is %f", temp);
  AddLog(LOG_LEVEL_INFO, "Stove water temp is %d", water);
  ResponseCmndDone();
}

void CmdStoveOn(void){
  stove.on();
  AddLog(LOG_LEVEL_INFO, PSTR("Micronova: Power Button pressed."));
  ResponseCmndDone();
}

void CmdStoveOff(void){
  stove.off();
  AddLog(LOG_LEVEL_INFO, PSTR("Micronova: Power Button pressed."));
  ResponseCmndDone();
}

void CmdStoveIR(void){
  stove.powerIR();
  AddLog(LOG_LEVEL_INFO, PSTR("Micronova: Power Button pressed."));
  ResponseCmndDone();
}

void CmdDbg(void){
  stove.on();

  AddLog(LOG_LEVEL_INFO, PSTR("DBG command"));
  ResponseCmndDone();
}

void CmdMeasurePellet(void) {
  digitalWrite( PELLET_TRIG, HIGH );
  delayMicroseconds(15);
  digitalWrite( PELLET_TRIG, LOW );
  pellet_echo_lenght = pulseIn(PELLET_ECHO, HIGH, 10000);
  pellet_cm = pellet_echo_lenght / 58;
  AddLog(LOG_LEVEL_INFO, "Pellet level measured %d", pellet_cm);
  ResponseCmndDone();
}

void CmdSendMQTT(void) {
  // AddLog(LOG_LEVEL_INFO, PSTR("Sending MQTT message."));
  // snprintf_P(topic, topic_size, PSTR("tasmota/myproject"));
  // snprintf_P(payload, payload_size, 
  //           PSTR("{\"" D_JSON_TIME "\":\"%s\",\"name\":\"My Project\"}"), 
  //           GetDateAndTime(DT_LOCAL).c_str()
  // );
  // // retain = true
  // MqttPublishPayload(topic, payload, strlen(payload), false);
  // ResponseCmndDone();

  MqttPublishPayload("Water Temperature", "10", 2, false);
  MqttPublishPayload("Pellet Loading Time", "20", 2, false);
  MqttPublishPayload("Status", "30", 2, false);
  MqttPublishPayload("Current Power", "40", 2, false);

  ResponseCmndDone();
}

void CmdHelp(void) {
  AddLog(LOG_LEVEL_INFO, PSTR("Help: Accepted commands - TogglePolling, SendMQTT, Help"));
  ResponseCmndDone();
  
}

/*********************************************************************************************\
 * Tasmota Functions
\*********************************************************************************************/

void MicronovaInit()
{
  /*
    Here goes My Project setting.
    Usually this part is included into setup() function
  */
  if (!initSuccess)
  {
    payload = (char *)calloc(payload_size, sizeof(char));
    topic = (char *)calloc(topic_size, sizeof(char));

    if (!payload || !topic)
    {
      AddLog(LOG_LEVEL_DEBUG, PSTR("Micronova: out of memory"));
      Serial.printf("INIT: Micronova out of memory");
      return;
    }

    if (stove.init())
      AddLog(LOG_LEVEL_INFO, PSTR("Micronova stove initialized"));
    else
      AddLog(LOG_LEVEL_INFO, PSTR("Micronova stove NOT initialized"));

    // GPIO INIT
    pinMode( PELLET_TRIG, OUTPUT );
    pinMode( PELLET_ECHO, INPUT );

    // MQTT

    initSuccess = true;
    AddLog(LOG_LEVEL_INFO, PSTR("Micronova init is successful..."));
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  AddLog(LOG_LEVEL_INFO, PSTR("Routine controllo camino"));
}

void MicronovaProcessing(void)
{

  /*
    Here goes My Project code.
    Usually this part is included into loop() function
  */

}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/
bool Xdrv107(uint32_t function)
{
  bool result = false;

  switch (function) {
    case FUNC_PRE_INIT:
      AddLog(LOG_LEVEL_INFO, PSTR("Micronova FUNC-PRE-INIT..."));
      //Serial.printf("Micronova FUNC-PRE-INIT...\n");
      break;
    case FUNC_INIT:
      AddLog(LOG_LEVEL_INFO, PSTR("Micronova FUNC-INIT..."));
      //Serial.printf("Micronova FUNC-INIT...\n");
      MicronovaInit();
      break;
//  case FUNC_EVERY_50_MSECOND:
//  case FUNC_EVERY_100_MSECOND:
//  case FUNC_EVERY_200_MSECOND:
  case FUNC_EVERY_250_MSECOND:
    // if (stove.RWrunning == false) {
    //   stoveResult = stove.read_and_store_all();
    //   if ( stoveResult != nullptr ) {
    //     //TODO: send MQTT message
    //     String payloadString = String(stoveResult->currentValue);
    //     payloadString.toCharArray(payload, payloadString.length()+1);
    //     MqttPublishPayload(stoveResult->topic, payload, strlen(payload), false);
    //   }
    // }
    //result = true;
    break;
  case FUNC_EVERY_SECOND:
    if (pelletMeasureDelay++ >= PELLET_MEASURE_PERIOD) {
      pelletMeasureDelay = 1;
      //stove.measurePellet();
    }
    if (stove.RWrunning == false) {
      //AddLog(LOG_LEVEL_INFO, PSTR("stove.RWrunning == false"));
      stoveResult = stove.read_and_store_topics();
      if ( stoveResult != -1 ) {
        AddLog(LOG_LEVEL_INFO, PSTR("stoveResult != -1"));
        //TODO: send MQTT message
        String topicString = String(stove.topics[stoveResult].topic);
        topicString.toCharArray(topic,topicString.length()+1);
        String payloadString = String(stove.topics[stoveResult].currentValue);
        payloadString.toCharArray(payload, payloadString.length()+1);
        MqttPublishPayload(topic, payload, strlen(payload), false);
        //MqttPublishPayload("Current Power", "70", 2, false);
      }
    }
    //result = true;
    break;
    
    case FUNC_COMMAND:    // Command support
      AddLog(LOG_LEVEL_INFO, PSTR("Calling Micronova Command..."));
      result = DecodeCommand(MicronovaCommands, MicronovaCommand);
      break;
  }
  return result;
}

#endif  // USE_MICRONOVA
