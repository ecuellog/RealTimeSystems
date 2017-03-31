CC = avr-gcc
OFLAGS = -DF_CPU=16000000 -mmcu=atmega2560
COPY = avr-objcopy
HEXFLAGS = -O ihex
FLASH = avrdude
FLASHFLAGS = -v -patmega2560 -cwiring -P/dev/ttyACM0 -b115200 -D -Uflash:w:

BASETARGET = base.hex
BASEOBJECTS = common/cswitch.S common/rtos.c common/utility.c baseStation/base.c baseStation/baseUtility.c
base: $(BASEOBJECTS)
	$(CC) $(OFLAGS) $(BASEOBJECTS) -o $(BASETARGET)
	$(COPY) $(HEXFLAGS) $(BASETARGET) $(BASETARGET)
	$(FLASH) $(FLASHFLAGS)$(BASETARGET)

ROOMBATARGET = roomba.hex
ROOMBAOBJECTS = common/cswitch.S common/rtos.c common/utility.c roombaStation/roomba.c roombaStation/roombaUtility.c roombaStation/roombaMotion.c
roomba: $(ROOMBAOBJECTS)
	$(CC) $(OFLAGS) $(ROOMBAOBJECTS) -o $(ROOMBATARGET)
	$(COPY) $(HEXFLAGS) $(ROOMBATARGET) $(ROOMBATARGET)
	$(FLASH) $(FLASHFLAGS)$(ROOMBATARGET)

clean:
	-rm -f *.o *.hex
