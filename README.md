# Explanation

# Compilation:

First compile to ARM (actually Thumb) instructions:
```
arm-none-eabi-gcc -o main.o -c -g -mcpu=cortex-m3 -mthumb main.c
```
Linker script arranges instructions in required memory configuration for device:
```
arm-none-eabi-gcc -o application.elf -Wl,-Tmemory.ld -nostartfiles main.o
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


