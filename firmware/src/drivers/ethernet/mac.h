#ifndef _CNETIO_MAC_
#define _CNETIO_MAC_

#include <commendef.h>

int mac_build_packet(uint8_t * packetBuffer, uint8_t * dest, uint8_t * src, uint8_t * payloadBuffer, uint16_t protocol, int payloadSize);

#endif