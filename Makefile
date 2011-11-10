OUT=quadstabilize
CPU_TYPE=cortex-m3

CROSS_COMPILE= arm-none-eabi-
AS= $(CROSS_COMPILE)gcc
CC= $(CROSS_COMPILE)gcc
LD= $(CROSS_COMPILE)gcc
SIZE= $(CROSS_COMPILE)size
OBJCOPY= $(CROSS_COMPILE)objcopy
OBJDUMP= $(CROSS_COMPILE)objdump

LPC_XPRESSO_DIR=/usr/local/lpcxpresso_4.1.0_164/lpcxpresso/bin
DFU=$(LPC_XPRESSO_DIR)/dfu-util
DFU_FLAGS=-d 0x471:0xdf55 -c 0 -t 2048 -R -D $(LPC_XPRESSO_DIR)/LPCXpressoWIN.enc
CRT=$(LPC_XPRESSO_DIR)/crt_emu_lpc11_13_nxp
CRT_FLAGS=-wire=winusb  -pLPC1343 -load-base=0x1000

CMSIS_INCLUDES=CMSIS/inc
LDLIBS=-lm
OCFLAGS=--strip-unneeded

CFLAGS=-c -O0 -I$(CMSIS_INCLUDES) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=cortex-m3 -fno-builtin -D__NEWLIB__
LD_FLAGS= -nostartfiles -mthumb -mcpu=$(CPU_TYPE) -Wl,--gc-sections
ASFLAGS = -c -O0 $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp

OBJFILES:=$(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst CMSIS/src/%.c,CMSIS/src/%.o,$(wildcard CMSIS/src/*.c)) $(patsubst core/%.c,core/%.o,$(wildcard core/*.c)) $(patsubst sensors/%.c,sensors/%.o,$(wildcard sensors/*.c))

all: firmware

firmware: $(OBJFILES)
	@ echo $(OBJFILES)
	$(LD) $(LD_FLAGS) -Tlinkscript.ld -o $(OUT).elf $(LDLIBS) $(OBJFILES)
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUT).elf $(OUT).bin
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OUT).elf $(OUT).hex

boot:
	$(DFU) $(DFU_FLAGS)

flash:
	$(CRT) $(CRT_FLAGS) --flash-load-exec=$(OUT).bin

clean:
	rm $(OBJFILES) $(OUT).elf $(OUT).bin $(OUT).hex

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o : %.s
	$(AS) $(ASFLAGS) -o $@ $<

