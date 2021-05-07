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

extern planet_t    planet;
extern satellite_t satellites[PLANET_MAX_SATELLITES];
extern esp_err_t addPeer(uint8_t *peerMAC);
extern esp_err_t esp_now_setup(void);
extern int sendPacket(uint8_t *, size_t, uint8_t *);

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


int searchSatellite(satellite_t* arr, int start,int count,uint8_t *mac) { 
    
    if( start < count)
    {
    	if( MAC_CMP(arr[start].sat_addr,mac))
        	return start;
    	return searchSatellite(arr,start+1,count,mac);
	}
    return -1;          
 }

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
        /*
        DEBUG_STR(" Destination Peer ");
        for (int i=0; i<MAC_ADDRESS_LEN; i++) {
            DEBUG_UIN(satellite->sat_addr[i],16);
            DEBUG_STR(":");
        }
        DEBUG_STR(", out packet type: "); 
        DEBUG_INT(packet_head.type); 
        */
    }
    else 
        DEBUG_STR("ERROR ");
    return retCode;
}

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
      /*
      DEBUG_STR(" data ");
      for (int i=0; i<planet.receive; i++) {
        Serial.print(planet.receive_buffer[i],HEX);
        Serial.print(":");
      }
      DEBUG_STRLN("");

      DEBUG_STR("planet.receive ");
      DEBUG_INT(planet.receive);
      DEBUG_STR(" bytes from ");
      for (int i=0; i<sizeof(packet_head.src_addr); i++) {
        Serial.print(packet_head.src_addr[i],HEX);
        Serial.print(":");
      }
      DEBUG_STR(", to ");
      for (int i=0; i<sizeof(packet_head.dest_addr); i++) {
        Serial.print(packet_head.dest_addr[i],HEX);
        Serial.print(":");
      }
      DEBUG_STR(" ");
      */
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
      /*
      DEBUG_STR(", in packet type: ");
      DEBUG_INT(packet_head.type);
      DEBUG_STR(" ");
      */
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
  /*
esp_err_t addPeer(uint8_t *peerMAC) {
    esp_now_peer_info_t *peer;
    esp_err_t retCode;

    if (!esp_now_is_peer_exist(peerMAC)) {
        peer = (esp_now_peer_info_t *) malloc(sizeof(esp_now_peer_info_t));
        if (peer == NULL)
            return 1;

        memset(peer, 0, sizeof(esp_now_peer_info_t));
        peer->channel = 0; //CONFIG_ESPNOW_CHANNEL;
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
*/

/*
int sendPacket(uint8_t * pkt,size_t pLen, uint8_t * destination) {

    int retCode = 1;

    retCode = esp_now_send(destination,  pkt, pLen);
    DEBUG_STR("send result: ");
    DEBUG_INT(retCode);
    DEBUG_STR(" ");
    return retCode;
}
*/
/*
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
        Serial.print(" Destination Peer ");
        for (int i=0; i<MAC_ADDRESS_LEN; i++) {
            Serial.print(satellite->sat_addr[i],HEX);
            Serial.print(":");
        }
        DEBUG_STR(", out packet type: "); 
        DEBUG_INT(packet_head.type); 
        DEBUG_STRLN(" ");
    }
    else 
        DEBUG_STRLN("ERROR ");
    return retCode;
}
*/
/*
    if (retCode == ESP_OK) {
        DEBUG_STR("Sending data: ");
        for(int i=0;i<sizeof(pkt);i++) {
        DEBUG_UCH(buffer[i],HEX);
        DEBUG_STR(" ");
        }
        DEBUG_STRLN("");
    }
    else  {
        if (planet.send_errors > 65534)
            planet.send_errors = 1;
        else
            planet.send_errors++;
//      ESP_LOGI(TAG, "data not sent, error: %d %d", retCode, satellite.reconnections);
    }
//      ESP_LOGI(TAG, "data sent: %d bytes.", pLen);
//  ESP_LOGI(TAG, "sendPacket -> MAC: %s", mac2str(destination, NULL));
*/
