all:
	arm-none-eabi-gcc *.c core/*.c -I/home/greghaynes/workspace/CMSISv2p00_LPC13xx/inc -I/home/greghaynes/workspace/LPC13xx_cmsis2_Lib/inc -D__USE_CMSIS=1 -D__NEWLIB__ -nostdlib -mcpu=cortex-m3 -mthumb -Tlink.ld
