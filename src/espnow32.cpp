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
 * @file espnow32.cpp
 *
 * espnow slave for esp32
 * 
 * Copyright (c) 2021 aRGi <info@argi.mooo.com>
 *
 */

#ifdef ESP32
#define PLANET_DEBUGON

#include <arduino.h>
#include <WiFi.h>
#include <esp_now.h>
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
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

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
void OnDataSent(const uint8_t * mac, esp_now_send_status_t status) {

}

/**
 * add peer to espnow peer list
 *
 * @param peerMac peer mac address
 *
 */
esp_err_t addPeer(uint8_t *peerMAC) {
    esp_now_peer_info_t *peer;
    esp_err_t retCode;

    if (!esp_now_is_peer_exist(peerMAC)) {
        peer = (esp_now_peer_info_t *) malloc(sizeof(esp_now_peer_info_t));
        if (peer == NULL)
            return 1;

        memset(peer, 0, sizeof(esp_now_peer_info_t));
        peer->channel = 1;
        peer->ifidx = ESP_IF_WIFI_STA;
        peer->encrypt = false;
        memcpy(peer->peer_addr, peerMAC, MAC_ADDRESS_LEN);
        retCode =  esp_now_add_peer(peer);
        free(peer);
        if (retCode != ESP_OK) {
            return 5;
        }
    }
    return ESP_OK;
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
    return retCode;
}

/**
 * setup espnow
 *
 */
esp_err_t esp_now_setup(void) {
  esp_err_t retcode;

  DEBUG_STR(" Mac address: ");
  esp_read_mac(planet.mac_addr, ESP_MAC_WIFI_STA);
  WiFi.mode(WIFI_MODE_STA);
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
  DEBUG_STRLN(" ");
  return retcode;
}
#endif
