#include <commendef.h>
#include <stdio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/ethernet/mac.h>
#include <libopencm3/ethernet/phy.h>
#include <libopencm3/ethernet/phy_lan87xx.h>
#include <core/clock.h>
#include <string.h>
#include <drivers/oled/oled.h>
#include <drivers/oled/font.h>
#include <libopencm3/stm32/syscfg.h>
#include <drivers/ethernet/mac.h>

#define PORT_SPI5 GPIOF 
#define PIN_SPI5_SCK GPIO7
#define PIN_SPI5_MISO GPIO8
#define PIN_SPI5_MOSI GPIO9

#define PORT_OLED 	GPIOE
#define PIN_DS 		GPIO5 
#define PIN_CS 		GPIO6 
#define PIN_RESET 	GPIO3 

//pc13 user button

static void gpio_port_w_bit(uint32_t port, uint16_t pin, uint16_t value){
	uint16_t portData = gpio_port_read(port);
	value = value & 1;
	if (value == 1){
		portData = portData | pin;
	}else{
		portData = portData & ~(pin);
	}
	gpio_port_write(port, portData);
}

static void build_header(uint8_t cnt){
	char str[80];

   	sprintf(str, "count: %d", cnt);

	for (int i = OLED_XSTART; i < OLED_WIDTH; i++)
	{
		for(int j = OLED_YSTART; j <= OLED_HEIGHT / 4; j++){
			oled_draw_pixel(i,j);
		}
		
	}
	oled_write_string(str, OLED_XSTART, OLED_YSTART + (OLED_HEIGHT / 8) - 4, &Font_7x10, 1);
	oled_refresh();
}

static void build_text(uint8_t state, bool link){
	for (int i = OLED_XSTART; i < OLED_WIDTH; i++)
	{
		for(int j = (OLED_YSTART + (OLED_HEIGHT / 4)) ; j <= OLED_HEIGHT; j++){
			oled_clear_pixel(i,j);
		}
		
	}
	if (state == 0x01){
		oled_write_string("Button on", OLED_XSTART + 1, OLED_YSTART + (OLED_HEIGHT / 4) + 5, &Font_11x18, 0);
	}else{
		oled_write_string("Button off", OLED_XSTART + 1, OLED_YSTART + (OLED_HEIGHT / 4) + 5, &Font_11x18, 0);
	}
	char str[80];
   	sprintf(str, "link: %d", link);
	oled_write_string(str, OLED_XSTART + 1, OLED_YSTART + (OLED_HEIGHT / 4) + 25, &Font_11x18, 0);
	oled_refresh();
}
      
#define SYSCFG_PMC_MII_RMII_SEL (0x1UL << (23U)) /*!< 0x00800000 */

int main(void)
{
	uint8_t binCount = 0;
	uint64_t NextTime = 0;
	uint64_t NextSend = 0;
	uint64_t CheckTime = 0;
	system_setup();
	rcc_periph_clock_enable(RCC_SYSCFG);
	rcc_periph_clock_enable(RCC_GPIOA); // etehenet
	rcc_periph_clock_enable(RCC_GPIOB); // user leds | etehenet
	rcc_periph_clock_enable(RCC_GPIOC); // user button | etehenet
	rcc_periph_clock_enable(RCC_GPIOE); // bits voor oled
	rcc_periph_clock_enable(RCC_GPIOF); //port spi5
	rcc_periph_clock_enable(RCC_GPIOG); //etehenet
	rcc_periph_clock_enable(RCC_SPI5); // port voor spi display
	rcc_periph_clock_enable(RCC_ETHMAC);
	rcc_periph_clock_enable(RCC_ETHMACRX);
	rcc_periph_clock_enable(RCC_ETHMACTX);
	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_DMA2);

	gpio_mode_setup(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO7 | GPIO0 | GPIO14);
	gpio_mode_setup(GPIOC,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,GPIO13);

	//AF 11 = ethernet
	gpio_mode_setup(GPIOA,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN, GPIO1 | GPIO2 | GPIO7);
	gpio_set_af(GPIOA, GPIO_AF11, GPIO1 | GPIO2 | GPIO7);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO1 | GPIO2 | GPIO7);

	gpio_mode_setup(GPIOB,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN, GPIO13);
	gpio_set_af(GPIOB, GPIO_AF11, GPIO13);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO13);

	gpio_mode_setup(GPIOC,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN, GPIO1 | GPIO4 | GPIO5);
	gpio_set_af(GPIOC, GPIO_AF11, GPIO1 | GPIO4 | GPIO5);
	gpio_set_output_options(GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO1 | GPIO4 | GPIO5);

	gpio_mode_setup(GPIOG,GPIO_MODE_AF,GPIO_PUPD_PULLDOWN, GPIO11 | GPIO13);
	gpio_set_af(GPIOG, GPIO_AF11, GPIO11 | GPIO13);
	gpio_set_output_options(GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, GPIO11 | GPIO13);

	SYSCFG_PMC &= ~(SYSCFG_PMC_MII_RMII_SEL);
	SYSCFG_PMC |= SYSCFG_PMC_MII_RMII_SEL;

	const uint8_t macAddr[6] = {
		0x5E,0x9A,0xBE,0x98,0xF8,0x22
	};

	uint8_t arppacket[28] = {
		0x00, //scource addr type
		0x01, //scource addr type
		0x08, //dest addr type
		0x00, //dest addr type
		0x06, //scource addr len
		0x04, //dest addr len
		0x00, //operation type
		0x01, //operation type
		macAddr[0],
		macAddr[1],
		macAddr[2],
		macAddr[3],
		macAddr[4],
		macAddr[5],
		0x0a, //10
		0x00, //0
		0x0a, //10
		0x19, //25
		0xff, //req mac
		0xff, //req mac
		0xff, //req mac
		0xff, //req mac
		0xff, //req mac
		0xff, //req mac
		0x0a, //10
		0x00, //0
		0x0a, //10
		0x18, //24
	};

	uint8_t	packet[50] = {0};

	#define nTx * (cTx + sz) + nRx * (cRx + sz)

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

	const uint8_t macDest[6] = {
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	};

	uint32_t packetLen = mac_build_packet(packet, macDest, macAddr, arppacket, 0x0806, 28);

	rcc_periph_reset_pulse(RST_ETHMAC);

	eth_init(PHY0, ETH_CLK_025_035MHZ);
	eth_desc_init(ethbuf,ETHBUF_NTX,ETHBUF_NRX,ETHBUF_CTX,ETHBUF_CRX,ETHBUG_EXT);
	eth_set_mac(macAddr);
	eth_enable_checksum_offload();
	eth_start();

	//AF 5 = SPI
	gpio_mode_setup(PORT_SPI5,GPIO_MODE_AF,GPIO_PUPD_NONE, PIN_SPI5_SCK | PIN_SPI5_MISO | PIN_SPI5_MOSI);
	gpio_mode_setup(PORT_OLED,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,PIN_DS | PIN_CS | PIN_RESET);
	gpio_set_af(PORT_SPI5, GPIO_AF5, PIN_SPI5_SCK | PIN_SPI5_MISO | PIN_SPI5_MOSI);
	
	spi_init_master(SPI5, SPI_CR1_BAUDRATE_FPCLK_DIV_8, SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE, SPI_CR1_CPHA_CLK_TRANSITION_1, SPI_CR1_DFF_8BIT, SPI_CR1_MSBFIRST);
	spi_enable_ss_output(SPI5);
	spi_enable(SPI5);

	oled_init();	
	oled_refresh();

	NextTime = system_get_ticks();
	NextSend = system_get_ticks();
	uint8_t lastState = 0x02;
	uint8_t newState = 0x00;
	bool lastLinkState = false;
	bool messageSend = false;
	CheckTime = system_get_ticks();
    while (1)
	{
		
		if (system_get_ticks() >= NextTime){
			NextTime = system_get_ticks() + 750;
			binCount++;
			if(binCount > 7){
				binCount = 0;
			}
			build_header(binCount);
		} 

		newState = gpio_port_read(GPIOC)>>13 & 1;
		if(newState != lastState || phy_link_isup(PHY0) != lastLinkState){
			lastState = newState;
			lastLinkState = phy_link_isup(PHY0);
			build_text(newState, lastLinkState);
		};
		
		if(phy_link_isup(PHY0) && system_get_ticks() >= NextSend){
			eth_tx(packet, packetLen);
			NextSend = system_get_ticks() + 4000;
		}
		
		
		
		/*/
			if(phy_link_isup(PHY0)){
				if(system_get_ticks() > (CheckTime + 5000)){
					
					oled_fill();
					oled_refresh();
					CheckTime = system_get_ticks();
				}
			}else{
				CheckTime = system_get_ticks();
			}
		*/
		

		gpio_port_w_bit(GPIOB, GPIO0, ((binCount) & 1));
		gpio_port_w_bit(GPIOB, GPIO7, ((binCount >> 1) & 1));
		gpio_port_w_bit(GPIOB, GPIO14, ((binCount >> 2) & 1));		
	}
	return 0;
}
