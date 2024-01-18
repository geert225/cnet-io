#include <drivers/ethernet/mac.h>
#include <string.h>
#include <libopencm3/ethernet/mac.h>

int mac_build_packet(uint8_t * packetBuffer, uint8_t * dest, uint8_t * src, uint8_t * payloadBuffer, uint16_t protocol, int payloadSize){
    int packetBufferSize = 14;
    memcpy(packetBuffer, dest, 6);
    memcpy(&packetBuffer[6], src, 6);
    packetBuffer[12] = (uint8_t)((protocol >> 8) & 0xFF);
    packetBuffer[13] = (uint8_t)(protocol & 0xFF);
    memcpy(&packetBuffer[14], payloadBuffer, payloadSize);
    packetBufferSize += payloadSize;
    return packetBufferSize;
}