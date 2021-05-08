/*
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

/**
* CHANGELOG
* 2021 05 07 
*    initial release
*/

/**
 * @file espnow8266.cpp
 *
 * espnow slave for esp8266
 * 
 * Copyright (c) 2021 aRGi <info@argi.mooo.com>
 *
 */

#ifdef ESP8266
#define PLANET_DEBUGON

/* some definition that are missing for esp8266 */
#define esp_err_t int
#define ESP_OK 0

#include <arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include "planet.h"
#include "debug.h"

extern planet_t planet;  // planet data

/**
 * callback called when data has been received by espnow
 *
 * @param mac receiver mac address
 * @param incomingData received data buffer
 * @param len received data buffer length
 *
 */
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8 len) {

  memcpy(planet.receive_buffer, incomingData, len); 
  planet.receive = len;

}

/**
 * callback called when data has been sent by espnow
 *
 * @param mac sender mac address
 * @param status send operation status
 *
 */
void OnDataSent(uint8_t * mac, uint8 status) {

}

/**
 * add peer to espnow peer list
 *
 * @param peerMac peer mac address
 *
 */
esp_err_t addPeer(uint8_t *peerMAC) {
    DEBUG_STR(" ADD Peer ");
    for (int i=0; i<MAC_ADDRESS_LEN; i++) {
        DEBUG_UIN(peerMAC[i],16);
        DEBUG_STR(":");
    }
    char pmk[] = "pmk1234567890123";
    return esp_now_add_peer(peerMAC, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

/**
 * send a packet to a peer using espnow protocol
 *
 * @param pkt packet to send
 * @param len packet lenght
 * @param destination destination mac address
 *
 */
int sendPacket(uint8_t * pkt,size_t pLen, uint8_t * destination) {

    int retCode = 1;

    retCode = esp_now_send(destination,  pkt, pLen);
    DEBUG_STR("send result: ");
    DEBUG_INT(retCode);
    DEBUG_STR(" ");
    return retCode;
}

/**
 * setup espnow
 *
 */
esp_err_t esp_now_setup(void) {
  esp_err_t retcode;

  DEBUG_STR(" Mac address: ");
  WiFi.macAddress(planet.mac_addr);
  WiFi.mode(WIFI_STA);
  for (int i=0; i<MAC_ADDRESS_LEN; i++) {
      DEBUG_UIN(planet.mac_addr[i],16);
      DEBUG_STR(":");
  }
  DEBUG_STR(", Channel: ");
  DEBUG_INT(WiFi.channel());

  retcode = esp_now_init();
  if (retcode != ESP_OK) {
    DEBUG_STRLN(" Error initializing ESP-NOW");
  }
  else {
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    retcode = esp_now_register_recv_cb(OnDataRecv);
    if (retcode != ESP_OK) {
      DEBUG_STRLN(" Error registering ESP-NOW receive cellback");
    }
    else {
      retcode = esp_now_register_send_cb(OnDataSent);
      if (retcode != ESP_OK) 
        DEBUG_STRLN(" Error registering ESP-NOW send cellback");
    }
  }
  retcode = addPeer(broadcastAddress);
  DEBUG_STRLN(" ");
  return retcode;
}
#endif
