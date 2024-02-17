/*
  user_config_override.h - user configuration overrides my_user_config.h for Tasmota

  Copyright (C) 2021  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

/*****************************************************************************************************\
 * USAGE:
 *   To modify the stock configuration without changing the my_user_config.h file:
 *   (1) copy this file to "user_config_override.h" (It will be ignored by Git)
 *   (2) define your own settings below
 *
 ******************************************************************************************************
 * ATTENTION:
 *   - Changes to SECTION1 PARAMETER defines will only override flash settings if you change define CFG_HOLDER.
 *   - Expect compiler warnings when no ifdef/undef/endif sequence is used.
 *   - You still need to update my_user_config.h for major define USE_MQTT_TLS.
 *   - All parameters can be persistent changed online using commands via MQTT, WebConsole or Serial.
\*****************************************************************************************************/

//
// Examples :
//

#define CASA_MIA
//#define CASA_ROSSANA
//#define CASA_MAMMA

#undef  CFG_HOLDER
#undef  STA_SSID1
#undef  STA_PASS1

// -- Master parameter control --------------------
#ifdef CASA_MIA
  #define CFG_HOLDER        4619                          // [Reset 1] Change this value to load SECTION1 configuration parameters to flash
  #define STA_SSID1         "PaciottiWiFi"                // [Ssid1] Wifi SSID
  #define STA_PASS1         "Paciott1Paciott1Paciott1"    // [Password1] Wifi password
  
  #undef  MQTT_HOST
  #define MQTT_HOST         "192.168.1.90"                // [MqttHost]
  #undef  MQTT_PORT
  #define MQTT_PORT         1883                          // [MqttPort] MQTT port (10123 on CloudMQTT)
  #undef  MQTT_USER
  #define MQTT_USER         "paciotti"                    // [MqttUser] Optional user
  #undef  MQTT_PASS
  #define MQTT_PASS         "tata1608"                    // [MqttPassword] Optional password
#endif
#ifdef CASA_ROSSANA
  #define CFG_HOLDER        4618                          // [Reset 1] Change this value to load SECTION1 configuration parameters to flash
  #define STA_SSID1         "Vodafone-C01860233"          // [Ssid1] Wifi SSID
  #define STA_PASS1         "zaP2z7HtXd4NKbyF"            // [Password1] Wifi password
#endif
#ifdef CASA_MAMMA
  #define CFG_HOLDER        4621                          // [Reset 1] Change this value to load SECTION1 configuration parameters to flash
  #define STA_SSID1         "TIM-18371019"                // [Ssid1] Wifi SSID
  #define STA_PASS1         "Paciott1Paciott1Paciott1"    // [Password1] Wifi password
#endif

//#define MICRONOVA_TX 9
//#define MICRONOVA_RX 10

// You might even pass some parameters from the command line ----------------------------
// Ie:  export PLATFORMIO_BUILD_FLAGS='-DUSE_CONFIG_OVERRIDE -DMY_IP="192.168.1.99" -DMY_GW="192.168.1.1" -DMY_DNS="192.168.1.1"'
/*
#ifdef MY_IP
#undef  WIFI_IP_ADDRESS
#define WIFI_IP_ADDRESS     MY_IP                // Set to 0.0.0.0 for using DHCP or enter a static IP address
#endif

#ifdef MY_GW
#undef  WIFI_GATEWAY
#define WIFI_GATEWAY        MY_GW                // if not using DHCP set Gateway IP address
#endif

#ifdef MY_DNS
#undef  WIFI_DNS
#define WIFI_DNS            MY_DNS               // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif

#ifdef MY_DNS2
#undef  WIFI_DNS2
#define WIFI_DNS2           MY_DNS2              // If not using DHCP set DNS IP address (might be equal to WIFI_GATEWAY)
#endif
*/
// !!! Remember that your changes GOES AT THE BOTTOM OF THIS FILE right before the last #endif !!!


// Own Configuration goes here
#ifdef PROJECT
  #undef PROJECT
  #define PROJECT           "Micronova"       // PROJECT is used as the default topic delimiter
#endif 

#ifdef MODULE
#undef MODULE
#endif
#define MODULE                 USER_MODULE   // [Module] Select default model (the list is kModuleNiceList() in file tasmota_template.h) USER_MODULE is the TEMPLATE

#ifdef FALLBACK_MODULE
#undef FALLBACK_MODULE
#endif
#define FALLBACK_MODULE        USER_MODULE   // to Select the default model as FALLBACK when the user does a RESET 1 

#ifdef USER_TEMPLATE
#undef USER_TEMPLATE
#endif
#define USER_TEMPLATE         "{\"NAME\":\"Micronova\",\"GPIO\":[1,1,1,1,3232,3200,1,1,1,1,1,1,1,1],\"FLAG\":0,\"BASE\":18}"  // [Template] Set JSON template

#endif  // _USER_CONFIG_OVERRIDE_H_
