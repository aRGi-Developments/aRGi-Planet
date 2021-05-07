
#define MAC_ADDRESS_LEN   6

static uint8_t broadcastAddress[MAC_ADDRESS_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#define IS_BROADCAST_ADDR(addr) (memcmp(addr, broadcastAddress, MAC_ADDRESS_LEN) == 0)
#define MAC_CMP(srcaddr,dstaddr) (memcmp(srcaddr, dstaddr, MAC_ADDRESS_LEN) == 0)

enum PacketTypes_e {
  PT_ADDRREQ         =  0b00000100,
  PT_ADDRREQ_OFFER   =  0b00000101,
  PT_ADDRREQ_REQ     =  0b00000110,
  PT_ADDRREQ_ACK     =  0b00000111,
  PT_TELEMETRY       =  0b00001100,
  PT_TELEMETRY_ACK   =  0b00001101,
};

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

typedef struct {
    uint8_t  type;
    uint8_t  src_addr[MAC_ADDRESS_LEN];
    uint8_t  dest_addr[MAC_ADDRESS_LEN];
} satellite_packet_head_t;

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

typedef struct {
    float temp;
    float humi;
    float pres;
} satellite_packet_payload_data_t;

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

/*
typedef struct {
    bool     free;
    uint8_t  sat_addr[MAC_ADDRESS_LEN];
    uint8_t  peer_addr[MAC_ADDRESS_LEN];
    float    temp;
    float    humi;
    float    pres;
    uint16_t uptime;
    uint16_t reconnections;
    int16_t  rssi;
    uint16_t timeouts;
    uint8_t  send;
    uint8_t  receive;
    volatile unsigned long  tmstart;
    volatile unsigned long  tmelapsed; 
    volatile unsigned long  tmduration; 
} satellite_t;
*/