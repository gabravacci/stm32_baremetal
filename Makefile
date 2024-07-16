CFLAGS  ?= -mcpu=cortex-m3 -mthumb $(EXTRA_CFLAGS)
LDFLAGS ?= -Wl,-Tlink.ld -nostartfiles
SOURCES = main.c 

build: firmware.elf

main.o: $(SOURCES)
	arm-none-eabi-gcc -o $@ -c -g $(CFLAGS) $(SOURCES)

firmware.elf: main.o
	arm-none-eabi-gcc -o $@ $(LDFLAGS) $<

firmware.bin: firmware.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: firmware.bin
	st-flash --reset write $< 0x8000000

clean:
	rm -rf firmware.*	
	rm main.o
