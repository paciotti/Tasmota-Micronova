#define USE_MICRONOVA
#ifdef USE_MICRONOVA

#warning **** Micronova Driver is included... ****

#define XDRV_107 107

/*********************************************************************************************\
 * Micronova Functions
\*********************************************************************************************/

// This variable will be set to true after initialization
bool initSuccess = false;

char * payload = nullptr;
size_t payload_size = 100;
char * topic = nullptr;
size_t topic_size = 30;

/* 
  Optional: if you need to pass any command for your device 
  Commands are issued in Console or Web Console
  Commands:
    TogglePolling  - Only prints some text. Can be made something more useful...
    SendMQTT   - Send a MQTT example message
    Help       - Prints a list of available commands  
*/

const char MicronovaCommands[] PROGMEM = "|"  // No Prefix
  "TogglePolling|" 
  "SendMQTT|"
  "Help";

void (* const MicronovaCommand[])(void) PROGMEM = {
  &CmdTogglePolling, &CmdSendMQTT, &CmdHelp };

void CmdTogglePolling(void) {
  AddLog(LOG_LEVEL_INFO, PSTR("Toggle Polling."));
  ResponseCmndDone();
}

void CmdSendMQTT(void) {
  AddLog(LOG_LEVEL_INFO, PSTR("Sending MQTT message."));

  snprintf_P(topic, topic_size, PSTR("tasmota/myproject"));

  snprintf_P(payload, payload_size, 
            PSTR("{\"" D_JSON_TIME "\":\"%s\",\"name\":\"My Project\"}"), 
            GetDateAndTime(DT_LOCAL).c_str()
  );

  // retain = true
  MqttPublishPayload(topic, payload, strlen(payload), false);

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

  payload = (char*)calloc(payload_size, sizeof(char));
  topic = (char*)calloc(topic_size, sizeof(char));

  if (!payload || !topic) {
    AddLog(LOG_LEVEL_DEBUG, PSTR("Micronova: out of memory"));
    return;
  }

  initSuccess = true;

  AddLog(LOG_LEVEL_DEBUG, PSTR("Micronova init is successful..."));

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

  if (FUNC_INIT == function) {
    MicronovaInit();
  }
  else if (initSuccess) {

    switch (function) {
      // Select suitable interval for polling your function
//    case FUNC_EVERY_SECOND:
      case FUNC_EVERY_250_MSECOND:
//    case FUNC_EVERY_200_MSECOND:
//    case FUNC_EVERY_100_MSECOND:
        MicronovaProcessing();
        break;

      // Command support
      case FUNC_COMMAND:
        AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("Calling Micronova Command..."));
        result = DecodeCommand(MicronovaCommands, MicronovaCommand);
        break;

    }

  }

  return result;
}

#endif  // USE_MICRONOVA
