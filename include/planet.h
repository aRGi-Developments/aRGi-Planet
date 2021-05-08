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
 * @file planet.h
 *
 * this file contains planet and satellites data definitions
 * 
 * Copyright (c) 2021 aRGi <info@argi.mooo.com>
 *
 */

#define MAC_ADDRESS_LEN   6   // espnow for esp32 defines this but is not defined on espnow for esp8266

// broadcast address as an array
static uint8_t broadcastAddress[MAC_ADDRESS_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// macro to check if the received packet come from a broadcast request or not
#define IS_BROADCAST_ADDR(addr) (memcmp(addr, broadcastAddress, MAC_ADDRESS_LEN) == 0)
// macro to compare two mac addresses
#define MAC_CMP(srcaddr,dstaddr) (memcmp(srcaddr, dstaddr, MAC_ADDRESS_LEN) == 0)

/* packet types
 *
 * PT_ADDRREQ        sent by satellites to request planet address
 * PT_ADDRREQ_OFFER  reply from planet to offer his address
 * PT_ADDRREQ_REQ    sent by satellites to confirm planet address receival
 * PT_ADDRREQ_ACK    acknowledge from planet
 * PT_TELEMETRY      sent by satellites to send data to planet
 * PT_TELEMETRY_ACK  planet data receival acknowledgement
 *
 */
enum PacketTypes_e {
  PT_ADDRREQ         =  0b00000100,
  PT_ADDRREQ_OFFER   =  0b00000101,
  PT_ADDRREQ_REQ     =  0b00000110,
  PT_ADDRREQ_ACK     =  0b00000111,
  PT_TELEMETRY       =  0b00001100,
  PT_TELEMETRY_ACK   =  0b00001101,
};

/* satellite data
 * free             this item in not assigned to a satellite
 * state            0 = free/unassigned
 *                  1 = assigned to this planet
 * sat_addr         satellite address aka unique identifier
 * peer_addr        planet addrees
 * uptime           satellite uptime
 * pktnum           number of packets sent by satellite
 * rssi             radio signal strenght
 * receive          not user in planet
 * receive_timeouts number of timouts when trying to receive data
 * receive_errors   number of receive errors
 * send             not user in planet
 * send_timeouts    number of timouts when trying to send data
 * send_errors      number of send errors
 * tmstart          not used in planet
 * tmelapsed        not used in planet
 * tmduration       not used in planet
 * temp             satellite temperature
 * humi             satellite humidity
 * pres             satellite pressure
*/
typedef struct {
    bool     free;
    uint8_t  state;
    uint8_t  sat_addr[MAC_ADDRESS_LEN];
    uint8_t  peer_addr[MAC_ADDRESS_LEN];
    uint16_t uptime;
    uint16_t pktnum;
    int16_t  rssi;
    uint8_t  receive;
    uint16_t receive_timeouts;
    uint16_t receive_errors;
    uint8_t  send;
    uint16_t send_timeouts;
    uint16_t send_errors;
    volatile unsigned long  tmstart;
    volatile unsigned long  tmelapsed; 
    volatile unsigned long  tmduration; 
    float    temp;
    float    humi;
    float    pres;
} satellite_t;

/* packet header
 * type      packet type see enum PacketTypes_e
 * sat_addr  satellite address aka unique identifier
 * dest_addr planet address or broadcast
*/
typedef struct {
    uint8_t  type;
    uint8_t  src_addr[MAC_ADDRESS_LEN];
    uint8_t  dest_addr[MAC_ADDRESS_LEN];
} satellite_packet_head_t;

/* packet control data
 * state            0 = free/unassigned
 *                  1 = assigned to this planet
 * command          if we need to send a command to the satellite (TBI)
 * uptime           satellite uptime
 * pktnum           number of packets sent by satellite
 * rssi             radio signal strenght
 * receive_timeouts number of timouts when trying to receive data
 * receive_errors   number of receive errors
 * send_timeouts    number of timouts when trying to send data
 * err_code         satellite last error
 * err_text         satellite last error description
*/
typedef struct {
    uint8_t state;
    uint8_t command;
    uint16_t uptime;
    uint16_t pktnum;
    int16_t  rssi;
    uint16_t receive_timeouts;
    uint16_t receive_errors;
    uint16_t send_timeouts;
    uint16_t send_errors;
    uint16_t err_code;
    uint8_t  err_text[40];
} satellite_packet_payload_t;

/* satellite sensors data
 * temp             satellite temperature
 * humi             satellite humidity
 * pres             satellite pressure
*/
typedef struct {
    float temp;
    float humi;
    float pres;
} satellite_packet_payload_data_t;

/* planet data
 * mac_addr         planet mac address
 * uptime           satellite uptime
 * timeouts         timeout counter to be removed
 * tmstart          timeouts counters
 * tmelapsed        
 * tmduration       
 * receive          some data received
 * receive_timeouts number of timouts when trying to receive data
 * receive_errors   number of receive errors
 * receive_buffer   where received data are placed by callback
 * send             data has been sent
 * send_timeouts    number of timouts when trying to send data
 * send_errors      number of send errors
 * send_buffer      
*/
typedef struct {
    uint8_t  mac_addr[MAC_ADDRESS_LEN];
    uint16_t uptime;
    uint16_t timeouts;
    volatile unsigned long  tmstart;
    volatile unsigned long  tmelapsed; 
    volatile unsigned long  tmduration; 
    uint8_t  receive;
    uint16_t receive_timeouts;
    uint16_t receive_errors;
    uint8_t* receive_buffer;
    uint8_t  send;
    uint16_t send_timeouts;
    uint16_t send_errors;
    uint8_t* send_buffer;
} planet_t;
