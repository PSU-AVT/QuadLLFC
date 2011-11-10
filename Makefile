OUT=quadstabilize
CPU_TYPE=cortex-m3

CROSS_COMPILE= arm-none-eabi-
AS= $(CROSS_COMPILE)gcc
CC= $(CROSS_COMPILE)gcc
LD= $(CROSS_COMPILE)gcc
SIZE= $(CROSS_COMPILE)size
OBJCOPY= $(CROSS_COMPILE)objcopy
OBJDUMP= $(CROSS_COMPILE)objdump

CMSIS_INCLUDES=CMSIS/inc
LDLIBS=
OCFLAGS=--strip-unneeded

CFLAGS=-c -g -O0 -I$(CMSIS_INCLUDES) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=cortex-m3 -fno-builtin
LD_FLAGS= -nostartfiles -mthumb -mcpu=$(CPU_TYPE) -Wl,--gc-sections
ASFLAGS = -c -g -O0 $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp

OBJFILES:=$(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst CMSIS/src/%.c,CMSIS/src/%.o,$(wildcard CMSIS/src/*.c)) $(patsubst core/%.c,core/%.o,$(wildcard core/*.c))

all: firmware

firmware: $(OBJFILES)
	@ echo $(OBJFILES)
	$(LD) $(LDFLAGS) -Tlinkscript.ld -o $(OUT).elf $(LDLIBS) $(OBJFILES)
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUT).elf $(OUT).bin
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OUT).elf $(OUT).hex

clean:
	rm *.o CMSIS/src/*.o core/*.o $(OUT).elf $(OUT).bin $(OUT).hex

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $<

