help:
	@echo 'Help details:'
	@echo 'hex: compile hex file'
	@echo 'flash: install hex file'
	@echo 'program: compile hex and install'

hex:
	avr-gcc -Os -DF_CPU=16000000 -mmcu=atmega2560 -c led_flash.c
	avr-gcc -DF_CPU=16000000 -mmcu=atmega2560 -o led_flash.elf led_flash.o
	avr-objcopy -O ihex led_flash.elf led_flash.hex
	rm led_flash.o
	rm led_flash.elf

flash:
	avrdude -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:led_flash.hex