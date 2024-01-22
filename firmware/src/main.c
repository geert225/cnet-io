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
#include <drivers/ethernet/mac.h>
#include <drivers/ethernet/basicProtocol.h>

#define PORT_SPI5 GPIOF 
#define PIN_SPI5_SCK GPIO7
#define PIN_SPI5_MISO GPIO8
#define PIN_SPI5_MOSI GPIO9

#define PORT_OLED 	GPIOE
#define PIN_DS 		GPIO5 
#define PIN_CS 		GPIO6 
#define PIN_RESET 	GPIO3 

#define PORT_ENCODER GPIOD
#define PIN_ENCODER_SWITCH GPIO5
#define PIN_ENCODER_DT GPIO6
#define PIN_ENCODER_CLK GPIO7

oledHandle_t oled1;

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
			oled_draw_pixel(&oled1, i,j);
		}
		
	}
	oled_write_string(&oled1, str, OLED_XSTART, OLED_YSTART + (OLED_HEIGHT / 8) - 4, &Font_7x10, 1);
	oled_refresh(&oled1);
}

static void build_text(uint8_t state, uint16_t link){
	for (int i = OLED_XSTART; i < OLED_WIDTH; i++)
	{
		for(int j = (OLED_YSTART + (OLED_HEIGHT / 4)) ; j <= OLED_HEIGHT; j++){
			oled_clear_pixel(&oled1, i,j);
		}
		
	}
	if (state == 0x01){
		oled_write_string(&oled1, "Button on", OLED_XSTART + 1, OLED_YSTART + (OLED_HEIGHT / 4) + 5, &Font_11x18, 0);
	}else{
		oled_write_string(&oled1, "Button off", OLED_XSTART + 1, OLED_YSTART + (OLED_HEIGHT / 4) + 5, &Font_11x18, 0);
	}
	char str[80];
   	sprintf(str, "enc: %d", link);
	oled_write_string(&oled1, str, OLED_XSTART + 1, OLED_YSTART + (OLED_HEIGHT / 4) + 25, &Font_11x18, 0);
	oled_refresh(&oled1);
}
      


int main(void)
{
	uint8_t binCount = 0;
	uint64_t NextTime = 0;
	uint64_t NextSend = 0;
	uint64_t CheckTime = 0;
	uint64_t ResetTime = 0;
	system_setup();
	

	gpio_mode_setup(GPIOB,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,GPIO7 | GPIO0 | GPIO14);
	gpio_mode_setup(GPIOC,GPIO_MODE_INPUT,GPIO_PUPD_PULLDOWN,GPIO13);

	gpio_mode_setup(PORT_ENCODER,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP, PIN_ENCODER_CLK | PIN_ENCODER_DT | PIN_ENCODER_SWITCH);

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
	
	uint8_t	packet[50] = {0};

	messageHandler_t *ethHandle;	

	macaddress_t deviceMac = {
		0x5E,0x9A,0xBE,0x98,0xF8,0x22
	};
	
	ethHandle = basic_protocol_handler_init(&deviceMac);

	macaddress_t reqMac = {
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	};
	bool hasReqMac = false;

	const uint8_t macDest[6] = {
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
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
		ethHandle->macAddress[0],
		ethHandle->macAddress[1],
		ethHandle->macAddress[2],
		ethHandle->macAddress[3],
		ethHandle->macAddress[4],
		ethHandle->macAddress[5],
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

	uint32_t packetLen = mac_build_packet(packet, macDest, deviceMac, arppacket, 0x0806, 28);

	

	oled_init(&oled1, SPI5, PIN_SPI5_MOSI, PIN_SPI5_SCK, PIN_CS, PIN_DS, PIN_RESET, PORT_SPI5, PORT_SPI5, PORT_OLED, PORT_OLED, PORT_OLED);	
	oled_refresh(&oled1);

	NextTime = system_get_ticks();
	NextSend = system_get_ticks();
	uint8_t lastState = 0x02;
	uint8_t newState = 0x00;
	uint16_t lastEncoderState = 0xFFFF;
	uint16_t counter = 0;
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
		if(newState != lastState || gpio_port_read(PORT_ENCODER) != lastEncoderState){
			lastState = newState;
			lastEncoderState = gpio_port_read(PORT_ENCODER);

			if(system_get_ticks() > ResetTime){
				uint16_t EncPort =  gpio_port_read(PORT_ENCODER);
				uint16_t clk = (EncPort >> 7) & 1;
				uint16_t dt = (EncPort >> 6) & 1;
				uint16_t swtch = (EncPort >> 5) & 1;
				if(clk == 0){
					ResetTime = system_get_ticks() + 25;
					if(dt == 0){
						counter++;
					}else{
						counter--;
					}
				}
			}
			build_text(newState, counter);
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
