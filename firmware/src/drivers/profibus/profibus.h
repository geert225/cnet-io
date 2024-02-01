#ifndef _CNETIO_PROFIBUS_
#define _CNETIO_PROFIBUS_

#include <commendef.h>

#define PROFITXBUF 512
#define PROFIRXBUF 512

#define PROFIRGBMODULECOUNT 4
#define PROFIRGBMODULESIZE 3
#define PROFIINPUTBYTECOUNT 1
#define PROFIOUTPUTBYTECOUNT 1

#define PROFIINPUTMODULESIZE PROFIINPUTBYTECOUNT
#define PROFIOUTPUTMODULESIZE PROFIOUTPUTBYTECOUNT + (PROFIRGBMODULECOUNT * PROFIRGBMODULESIZE)

typedef enum __attribute__((__packed__)) {
    waitingForSync,
    waitingForData,
    readingDataPacket,
    writeingDataPacket
} profibusStatus_t;

typedef struct 
{
    uint32_t uartPeriph;
    uint8_t txBuffer[PROFITXBUF];
    uint32_t txBufferLength;
    uint8_t rxBuffer[PROFIRXBUF];
    uint32_t rxBufferLength;
    uint8_t address;
    profibusStatus_t status;
    uint8_t sync;
    uint8_t freeze;
    uint8_t diag;
    uint8_t outputModuleData[PROFIOUTPUTMODULESIZE];
    uint8_t inputModuleData[PROFIINPUTMODULESIZE];
} profibusHandle_t;

#endif