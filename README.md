# cnet-io 
 

## what is this project?
 this project is all about the learning procces of how to build firmware for 32 bit micro controllers and trying to understand how industrial communication protocols work. For this board i have chosen a stm 32 Fxxx micro controller and the following protocols: Profibus, Profinet and Fanuc i/o link. The end goal is that i have a board that is to be able to be controlled by a plc or a robot controller.

## project status

| item    | Progress |
| :--------: | ------- |
| **board design** | ![](https://geps.dev/progress/1?dangerColor=6488ea&warningColor=6488ea&successColor=6488ea) |
| **firmware** | ![](https://geps.dev/progress/1?dangerColor=6488ea&warningColor=6488ea&successColor=6488ea) |
| **doc** | ![](https://geps.dev/progress/0?dangerColor=6488ea&warningColor=6488ea&successColor=6488ea) |

## todo list
#### board:
- [ ] choose controller (stm 32) ![](https://img.shields.io/badge/active-6488ea) 
- [ ] ethernet (spi? phy? integrated?) ![](https://img.shields.io/badge/active-6488ea) 
- [ ] Uart to rs485 2x
- [ ] power -> 3.3v, 3.3va, 5v, 12v, 24v(input)
- [ ] optical isolated inputs (min 4x)
- [ ] relay outputs (min 2x)
- [ ] neo pixel (ws2811, ws2812) outputs (min 4x)
- [ ] servo / pwm optical isolated outputs (min 2x)

#### firmware:
- [ ] make files and linker scripts ![](https://img.shields.io/badge/active-6488ea) 
- [ ] basic boot setup
- [ ] io control
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


