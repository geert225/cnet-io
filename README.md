# cnet-io 
 

## what is this project?
 this project is all about the learning procces of how to build firmware for 32 bit micro controllers and trying to understand how industrial communication protocols work. For this board i have chosen a stm 32 F217VET6 micro controller and the following protocols: Profibus, Profinet and Fanuc i/o link. The end goal is that i have a board that is to be able to be controlled by a plc or a robot controller.

## project status

| item    | Progress |
| :--------: | ------- |
| **board design** | ![](https://geps.dev/progress/1?dangerColor=6488ea&warningColor=6488ea&successColor=6488ea) |
| **firmware** | ![](https://geps.dev/progress/10?dangerColor=6488ea&warningColor=6488ea&successColor=6488ea) |
| **doc** | ![](https://geps.dev/progress/0?dangerColor=6488ea&warningColor=6488ea&successColor=6488ea) |

#### firmware status done:
- first setup of a stm 32 dev board
- first spi driver for oled screen
- start with ethernet driver
- free rtos setup (active)
- profinet packet inspection

## todo list
#### board:
- [X] choose controller (stm 32)
- [X] ethernet (spi? phy? integrated?) 
- [ ] Uart to rs485 2x
- [ ] power -> 3.3v, 3.3va, 5v, 12v, 24v(input) ![](https://img.shields.io/badge/active-6488ea) 
- [ ] optical isolated inputs (min 4x) ![](https://img.shields.io/badge/active-6488ea) 
- [ ] relay outputs (min 2x) ![](https://img.shields.io/badge/active-6488ea) 
- [ ] neo pixel (ws2811, ws2812) outputs (min 4x) ![](https://img.shields.io/badge/active-6488ea) 
- [ ] servo / pwm optical isolated outputs (min 2x) ![](https://img.shields.io/badge/active-6488ea) 

#### firmware:
- [x] make files and linker scripts 
- [ ] basic boot setup ![](https://img.shields.io/badge/active-6488ea) 
- [ ] io control ![](https://img.shields.io/badge/active-6488ea) 
- [ ] Uart control
- [ ] ethernet control
- [ ] Profibus protocol
- [ ] Profinet protocol ![](https://img.shields.io/badge/active-6488ea) 
- [ ] S7 protocol ? (nice to have)
- [ ] Fanuc i/o link protocol ? (no doc available)

## used tools
for this project i have used the following tools:

electrical design:
* kicad
* ST MCUfinder

programming:
* gcc compiler
* vs-code
* gnu make
* arm build tools

used hardware:
* st link

## used resources
for this project i have used a couple resources.

Low Byte Productions Bare Metal Programming Series
for the basics of programming stm 32 micro controllers with the Libopencm3 hal


