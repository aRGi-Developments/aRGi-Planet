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
 * @file planet.cpp
 *
 * this file contains functions to run the planet
 * 
 * Copyright (c) 2021 aRGi <info@argi.mooo.com>
 *
 */

// comment out the line below to stop logging to terminal
#define PLANET_DEBUGON

#include <arduino.h>
#ifdef ESP32
  #include <esp_now.h>
#endif  
#ifdef ESP8266
  #include <espnow.h>
  #define esp_err_t int
  #define ESP_OK 0
  #define esp_now_send_status_t uint8_t
#endif  

#include "planet.h"
#include "debug.h"

#define PACKET_MAXLEN   250
#define PLANET_MAX_SATELLITES  20

extern planet_t    planet;                              // planet data
extern satellite_t satellites[PLANET_MAX_SATELLITES];   // satellites data
extern esp_err_t addPeer(uint8_t *peerMAC);              
extern esp_err_t esp_now_setup(void);
extern int sendPacket(uint8_t *, size_t, uint8_t *);

/**
 * startup, allocates send and receive buffers, configures espnow
 *
 */
esp_err_t planetStartup(void) {

  DEBUG_STR("Planet Startup: ");
  planet.send = 0;
  planet.receive = 0;

  planet.receive_buffer = (uint8_t*) malloc( PACKET_MAXLEN );
  if (planet.receive_buffer == NULL) {
    DEBUG_STRLN("Malloc receive buffer fail");
    return 1;
  }
  planet.send_buffer = (uint8_t*) malloc( PACKET_MAXLEN );
  if (planet.send_buffer == NULL) {
    DEBUG_STRLN("Malloc send buffer fail");
    return 2;
  }
  for (int cnt=0; cnt < PLANET_MAX_SATELLITES; cnt++) {
    memset(&satellites[cnt], 0, sizeof(satellite_t));
    satellites[cnt].free = true;
  }
  return esp_now_setup();
}

/**
 * search for a known satellite, recursive search
 *
 * @param arr satellites data
 * @param start index from which begin searching
 * @param count maximum number of satellites this planet can handle
 * @param mac satellite mac address, aka saltellite unique identifier
 *
 */
int searchSatellite(satellite_t* arr, int start,int count,uint8_t *mac) { 
    
    if( start < count)
    {
    	if( MAC_CMP(arr[start].sat_addr,mac))
        	return start;
    	return searchSatellite(arr,start+1,count,mac);
	}
    return -1;          
 }

/**
 * add a satellite to the planet 
 *
 * @param arr satellites data
 * @param host planet data, we use this to properly configure satelite data
 * @param newSat satellite data
 *
 */
 int addSatellite(satellite_t* arr, planet_t *host, satellite_packet_head_t* newSat) { 
    
    int added = -1;
    int cnt=0;
    esp_err_t espnowError;

    while (added == -1 && cnt < PLANET_MAX_SATELLITES) {
        if (arr[cnt].free) {
            arr[cnt].free = false;
            espnowError = addPeer(newSat->src_addr);
            if (espnowError != ESP_OK)
                return PLANET_MAX_SATELLITES + espnowError;
            memcpy( arr[cnt].sat_addr, newSat->src_addr, MAC_ADDRESS_LEN);
            memcpy( arr[cnt].peer_addr, host->mac_addr, MAC_ADDRESS_LEN);
            arr[cnt].temp = 0;
            arr[cnt].humi = 0;
            arr[cnt].pres = 0;
            added = cnt;
        }
        else
            cnt++;
    }
    return added;          
 }

/**
 * send a message using espnow to a satellite
 *
 * @param satellite to send data to
 * @param host planet data
 * @param msgType message type
 *
 */
int sendMessage(satellite_t *satellite, planet_t *host, enum PacketTypes_e msgType) {

    satellite_packet_head_t packet_head;
    satellite_packet_payload_t packet_payload;

    size_t buffSize;
    static uint8_t * buff2send;
    uint16_t crc;
    esp_err_t retCode = 0xff;

    memcpy(packet_head.src_addr, host->mac_addr, MAC_ADDRESS_LEN);
    memcpy(packet_head.dest_addr, satellite->sat_addr, MAC_ADDRESS_LEN);
    packet_head.type = msgType;

    packet_payload.state = 0;
    packet_payload.command = 0;

    buffSize = sizeof(packet_head) + sizeof(satellite_packet_payload_t) + sizeof(uint16_t);

    buff2send = (uint8_t*) malloc(buffSize);
    if (!(buff2send == NULL)) {
        memset(buff2send, 0, buffSize);
        memcpy(buff2send, &packet_head, sizeof(satellite_packet_head_t));
        memcpy(&buff2send[sizeof(satellite_packet_head_t)], &packet_payload, sizeof(satellite_packet_payload_t));
        crc = 1; //crc16_le(UINT16_MAX, (uint8_t const *)buff2send, buffSize);
        memcpy(&buff2send[sizeof(satellite_packet_head_t)+sizeof(satellite_packet_payload_t)], &crc, sizeof(uint16_t));
        retCode = sendPacket(buff2send, buffSize, satellite->sat_addr);
        free(buff2send);
    }
    else 
        DEBUG_STR("ERROR ");
    return retCode;
}

/**
 * planet loop, wait for an incoming message, decode it and reply with the appropiate response
 *
 */
esp_err_t planetLoop() {
  satellite_packet_head_t packet_head;
  satellite_packet_payload_t packet_payload;
  satellite_packet_payload_data_t packet_data;
  int found;

  if (planet.receive != 0) {
    if (planet.receive > 0) {
      memcpy(&packet_head, planet.receive_buffer, sizeof(satellite_packet_head_t)); 
      memcpy(&packet_payload, &planet.receive_buffer[sizeof(satellite_packet_head_t)], sizeof(satellite_packet_payload_t)); 
      memcpy(&packet_data, &planet.receive_buffer[sizeof(satellite_packet_head_t)+sizeof(satellite_packet_payload_t)], sizeof(satellite_packet_payload_data_t)); 
      DEBUG_STR("Node ");
      found = searchSatellite(satellites, 0, PLANET_MAX_SATELLITES -1, packet_head.src_addr);
      if (found == -1) 
        found = addSatellite(satellites, &planet, &packet_head);
      DEBUG_UIN(found, HEX);
      if (found < 0 || found >= PLANET_MAX_SATELLITES) {
        DEBUG_STR(" not added ");
      }
      else {
        DEBUG_STR(" found ");
      }
      if (IS_BROADCAST_ADDR(packet_head.dest_addr)) {
        DEBUG_STR(", BROADCAST ");
      }
      DEBUG_STR(", uptime ");
      DEBUG_INT(packet_payload.uptime);
      DEBUG_STR(", temp ");
      DEBUG_DBL(packet_data.temp);
      DEBUG_STR(", humi ");
      DEBUG_DBL(packet_data.humi);
      DEBUG_STR(", pres ");
      DEBUG_DBL(packet_data.pres);
      DEBUG_STR(" ");

      if (!(found < 0 || found >= PLANET_MAX_SATELLITES)) {
        if (packet_head.type == PT_ADDRREQ || packet_head.type == PT_ADDRREQ_REQ || packet_head.type == PT_TELEMETRY) {
          sendMessage(&satellites[found], &planet, ((packet_head.type == PT_ADDRREQ) ? PT_ADDRREQ_OFFER : ((packet_head.type == PT_ADDRREQ_REQ) ? PT_ADDRREQ_ACK : PT_TELEMETRY_ACK)));
        }
      }

      DEBUG_STRLN("");
    }
    planet.receive = 0;
  }
  return planet.receive;
}
