OUT=quadstabilize
CPU_TYPE=cortex-m3

CROSS_COMPILE= arm-none-eabi-
AS= $(CROSS_COMPILE)gcc
CC= $(CROSS_COMPILE)gcc
LD= $(CROSS_COMPILE)gcc
SIZE= $(CROSS_COMPILE)size
OBJCOPY= $(CROSS_COMPILE)objcopy
OBJDUMP= $(CROSS_COMPILE)objdump

LPC_XPRESSO_DIR=/usr/local/lpcxpresso_4.2.2_238/lpcxpresso/bin
DFU=$(LPC_XPRESSO_DIR)/dfu-util
DFU_FLAGS=-d 0x471:0xdf55 -c 0 -t 2048 -R -D $(LPC_XPRESSO_DIR)/LPCXpressoWIN.enc
CRT=$(LPC_XPRESSO_DIR)/crt_emu_lpc11_13_nxp
CRT_FLAGS=-g -2 -pLPC1343 -wire=winUSB 

OCFLAGS=--strip-unneeded

CFLAGS=-g -c -I./ -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=cortex-m3 -DTARGET=LPC13xx -D__NEWLIB__ -O0
LD_FLAGS= -nostartfiles -mthumb -mcpu=$(CPU_TYPE) -Wl,--gc-sections -lm
ASFLAGS = -c -O0 $(INCLUDE_PATHS) -Wall -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=$(CPU_TYPE) -D__ASSEMBLY__ -x assembler-with-cpp

OBJFILES:=$(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst startup/%.c,startup/%.o,$(wildcard startup/*.c)) $(patsubst core/%.c,core/%.o,$(wildcard core/*.c)) $(patsubst sensors/%.c,sensors/%.o,$(wildcard sensors/*.c))

all: firmware

firmware: $(OBJFILES)
	@ echo $(OBJFILES)
	$(LD) -Tstartup/memory.ld -o $(OUT).elf $(OBJFILES) $(LD_FLAGS)
	$(OBJCOPY) $(OCFLAGS) -O binary $(OUT).elf $(OUT).bin
	$(OBJCOPY) $(OCFLAGS) -O ihex $(OUT).elf $(OUT).hex

debug:
	arm-none-eabi-gdb $(OUT).elf -x gdb_config

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

