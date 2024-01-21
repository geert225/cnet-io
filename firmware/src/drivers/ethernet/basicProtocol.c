#include <drivers/ethernet/basicProtocol.h>
#include <libopencm3/ethernet/mac.h>
#include <libopencm3/ethernet/phy.h>
#include <libopencm3/ethernet/phy_lan87xx.h>
#include <libopencm3/stm32/rcc.h>

#define ETHBUG_EXT 1 //if 1 extedded used
#if ETHBUG_EXT == 1
    #define ETHBUF_EXTDES 32
#else
    #define ETHBUF_EXTDES 16
#endif
#define ETHBUF_NTX 4 //number of transmitor desciptors
#define ETHBUF_CTX 256 //size of transmitor desciptors
#define ETHBUF_NRX 4 //number of recive desciptors
#define ETHBUF_CRX 256 //size of recive desciptors
#define ETHBUF_SIZE ((ETHBUF_NTX * (ETHBUF_CTX + ETHBUF_EXTDES)) + (ETHBUF_NRX * (ETHBUF_CRX + ETHBUF_EXTDES)))

uint8_t ethbuf[ETHBUF_SIZE];

messageHandler_t* basic_protocol_handler_init(macaddress_t *mac){
    messageHandler_t msgHandler = {
        .macAddress = mac,
        .ipAddress = {0,0,0,0},
        .protocol = ETH_PROTOCOL_NONE,
        .flagIpSet = false,
        .flagReqAct = false,
        .flagResponse = false
    };
    rcc_periph_reset_pulse(RST_ETHMAC);

	eth_init(PHY0, ETH_CLK_025_035MHZ);
	eth_desc_init(ethbuf,ETHBUF_NTX,ETHBUF_NRX,ETHBUF_CTX,ETHBUF_CRX,ETHBUG_EXT);
	eth_set_mac(mac);
	eth_enable_checksum_offload();
	eth_start();
    return &msgHandler;
}

void basic_protocol_handler_send(messageHandler_t* msgHandler){

}