This is a personal project for *learning* purposes. The goal is to implement a convenient and compact HAL for the STM32f103c8t6 microcontroller in the 'blue pill' package format (which I use very often).

I am heavily relying on these resources:

- https://github.com/cpq/bare-metal-programming-guide?tab=readme-ov-file
- https://maldus512.medium.com/bare-metal-programming-on-an-stm32f103-3a0f4e50ca29

As well as the appropriate datasheets, courtesy of STM.

# Features
- [x] Add a Makefile or build system for this
- [ ] GPIO configuration functions
- [ ] I2C functionality
- [ ] SPI functionality
- [ ] Drivers for common devices I use
    - [ ] LCD display driver
    - [ ] OLED (I2C) display driver
    - [ ] PWM pin driver
    - [ ] Accelerometer driver

# Compilation:

First compile to ARM (actually Thumb) instructions:
```
arm-none-eabi-gcc -o main.o -c -g -mcpu=cortex-m3 -mthumb main.c
```
Linker script arranges instructions in required memory configuration for device:
```
arm-none-eabi-gcc -o application.elf -Wl,-Tlink.ld -nostartfiles main.o
```
Finally need to compile `.elf` to actual binary that can be loaded to the FLASH memory:
```
arm-none-eabi-objcopy -O binary application.elf application.bin
```
Flashing (using ST-link) is done with:
```
st-flash write <firmware binary> <flash memory address to load>
```
Or in this case:
```
st-flash write application.bin 0x08000000
```
Recall this is the *physical* address of flash memory on the device.


