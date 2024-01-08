/*
  xdrv_107_micronova.ino - Micronova for Tasmota
*/

#ifdef USE_MICRONOVA
/*********************************************************************************************\
 * My IoT Device bare minimum
 *
 *
\*********************************************************************************************/

// #warning **** My IoT Device Driver is included... ****

#define XDRV_107 107

/*********************************************************************************************\
 * Tasmota Functions
\*********************************************************************************************/

// This variable will be set to true after initialization
bool initSuccess = false;

void MicronovaInit()
{

  /*
    Here goes My Project setting.
    Usually this part is included into setup() function
  */

  AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("Micronova init..."));

  Serial.begin(115200);

  // Set initSuccess at the very end of the init process
  // Init is successful
  initSuccess = true;
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
    AddLog(LOG_LEVEL_DEBUG_MORE, PSTR("Micronova init is done..."));
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
    }
  }
  return result;
}

#endif  // USE_MICRONOVA
