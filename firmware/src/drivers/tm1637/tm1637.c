#include <drivers/tm1637/tm1637.h>
#include <libopencm3/stm32/gpio.h>
#include <drivers/gpio/gpio.h>
#include <core/clock.h>

#define DATA_CMD (0x40)
#define DISPLAY_CMD (0x80)
#define ADDR_CMD (0xC0)

#define FIXEDADDR (0x04)

#define ZEROADDR ADDR_CMD
#define DISPON (1 << 3)
#define DISPOFF 0 

#define BITDELAY 80

static void tm1637_delay_us(uint16_t us){
    int cycles = us * BITDELAY;
    while (cycles > 0)
  {
    cycles--;
    __asm__ __volatile__("nop; nop; nop; nop;");
  }
}

static void tm1637_stop(tm1637Handle_t * handle){
    gpio_clear(handle->clockPort, handle->clockPin);
    tm1637_delay_us(2);
    gpio_clear(handle->dataPort, handle->dataPin);
    tm1637_delay_us(2);
    gpio_set(handle->clockPort, handle->clockPin);
    tm1637_delay_us(2);
    gpio_set(handle->dataPort, handle->dataPin);
}   

static void tm1637_start(tm1637Handle_t * handle){
    gpio_set(handle->clockPort, handle->clockPin);
    gpio_set(handle->dataPort, handle->dataPin);
    tm1637_delay_us(2);
    gpio_clear(handle->dataPort, handle->dataPin);
}

static uint8_t tm1637_ack(tm1637Handle_t * handle){
    gpio_clear(handle->clockPort, handle->clockPin);
    tm1637_delay_us(5);
    //uint8_t ack = gpio_port_read(handle->dataPort) & (handle->dataPin);
    gpio_set(handle->clockPort, handle->clockPin);
    tm1637_delay_us(2);
    gpio_clear(handle->clockPort, handle->clockPin);
    system_delay(1);
    return 1;
}

static void tm1637_send(tm1637Handle_t * handle, uint8_t data){
    for (uint8_t i = 0; i < 8; i++){
        gpio_clear(handle->clockPort, handle->clockPin);
        gpio_write_bit(handle->dataPort, handle->dataPin, ((data >> i) & 0x01));
        tm1637_delay_us(3);
        gpio_set(handle->clockPort, handle->clockPin);
        tm1637_delay_us(3);
    }
}

void tm1637_init(tm1637Handle_t * handle, uint16_t dataPin, uint16_t clockPin, uint32_t dataPort, uint32_t clockPort){
    handle->clockPin = clockPin;
    handle->clockPort = clockPort;
    handle->dataPin = dataPin;
    handle->dataPort = dataPort;
    for (int i = 0; i < 6; i++)
    {
        handle->dataBuffer[i] = 0xff;
    }
    gpio_mode_setup(handle->clockPort,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,handle->clockPin);
    gpio_mode_setup(handle->dataPort,GPIO_MODE_OUTPUT,GPIO_PUPD_NONE,handle->dataPin);

    gpio_set_output_options(handle->clockPort, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, handle->clockPin);
    gpio_set_output_options(handle->dataPort, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, handle->dataPin);

    gpio_clear(handle->clockPort, handle->clockPin);
    gpio_clear(handle->dataPort, handle->dataPin);
    
    tm1637_start(handle);
    tm1637_send(handle, 0x40);
    tm1637_ack(handle);
    tm1637_stop(handle);
    
    tm1637_start(handle);
    tm1637_send(handle, 0xc0);
    tm1637_ack(handle);
    for(int i=0; i<6; i++)
    {
    
    }
    tm1637_send(handle, 0xff);
    tm1637_ack(handle);
    tm1637_stop(handle);
    
    tm1637_start(handle);
    tm1637_send(handle, 0x8F);
    tm1637_ack(handle);
    tm1637_stop(handle);

}

void tm1637_clear(tm1637Handle_t * handle){
    for (int i = 0; i < 6; i++)
    {
        handle->dataBuffer[i] = 0;
    }
}

void tm1637_led_test(tm1637Handle_t * handle){
    for (int i = 0; i < 6; i++)
    {
        handle->dataBuffer[i] = 255;
    }
}

void tm1637_refresh(tm1637Handle_t * handle){
    //set address mode in auto inc
    tm1637_start(handle);
    tm1637_send(handle, DATA_CMD);
    tm1637_ack(handle);
    tm1637_stop(handle);
    //set addr to first one and write data
    tm1637_start(handle);
    tm1637_send(handle, ADDR_CMD | 0X00);
    tm1637_ack(handle);
    for (int i = 0; i < 6; i++)
    {
        tm1637_send(handle, handle->dataBuffer[i]);
        tm1637_ack(handle);
    }
    tm1637_stop(handle);
}

void tm1637_display_on(tm1637Handle_t * handle){
    tm1637_start(handle);
    tm1637_send(handle, 0x8f);
    tm1637_ack(handle);
    tm1637_stop(handle);
}

void tm1637_display_off(tm1637Handle_t * handle){
    tm1637_start(handle);
    tm1637_send(handle, DISPLAY_CMD | DISPOFF);
    tm1637_ack(handle);
    tm1637_stop(handle);
}