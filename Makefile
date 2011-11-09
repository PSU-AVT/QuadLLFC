CMSIS_INCLUDES=CMSIS/inc
CMSIS_LIB_INCLUDES=CMSIS/inc
CC=arm-none-eabi-gcc
OUT=quadstabilize

all:
	$(CC) *.c core/*.c CMSIS/src/* -I$(CMSIS_INCLUDES) -I$(CMSIS_LIB_INCLUDES) -D__USE_CMSIS=1 -D__NEWLIB__ -nostdlib -mcpu=cortex-m3 -mthumb -Tlinkscript.ld -o $(OUT)

clean:
	rm $(OUT)
