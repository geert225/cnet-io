#ifndef _CNETIO_BEP_
#define _CNETIO_BEP_

#include <commendef.h>

enum ETH_PROTOCOL {
    ETH_PROTOCOL_NONE = 0x0000,
    ETH_PROTOCOL_ARP = 0x0806,
    ETH_PROTOCOL_IPV4 = 0x0800,
    ETH_PROTOCOL_IPV6 = 0x86DD,
    ETH_PROTOCOL_PROFINET = 0x8892,
    ETH_PROTOCOL_LLDP = 0x88CC,
    ETH_PROTOCOL_WAKEONLAN = 0x0842
};

typedef uint8_t macaddress_t[6];
typedef uint8_t ipv4address_t[4];

typedef struct 
{
    macaddress_t * macAddress;
    ipv4address_t * ipAddress;
    uint16_t protocol; 
    bool flagReqAct;
    bool flagIpSet;
    bool flagResponse;
} messageHandler_t;

messageHandler_t* basic_protocol_handler_init(macaddress_t *mac);

#endif