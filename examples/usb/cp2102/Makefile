CPU = STM32F103RB
PRG = main

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
LD = arm-none-eabi-ld
AS = arm-none-eabi-as
SIZE = arm-none-eabi-size

INC = -I. -I../.. -I../../.. -I../../../../ -I../../../../drivers

CFLAGS = $(INC) -Os -g -mcpu=cortex-m3 -mthumb -nostartfiles -ffunction-sections -fdata-sections -DSTM32F10X_MD
CXXFLAGS = $(CFLAGS) -std=c++11 -fno-unwind-tables -fno-exceptions
LDFLAGS = -nostdlib --gc-sections -L../../.. -L../../../../libaeabi-cortexm0 -T../../../stm32f103xb.ld
LIBS = -laeabi-cortexm0

CPP_SRCS = main.cpp
C_SRCS = ../../../vectors.c ../../../utils.c

OBJS = $(CPP_SRCS:.cpp=.o) $(C_SRCS:.c=.o) $(ARM_SRCS:.s=.o)
HDRS = $(wildcard *.h)
INTERMEDIATES = $(PRG).lst $(PRG).bin $(PRG).elf

all: $(OBJS)
	$(LD) -o $(PRG).elf $(LDFLAGS) $(OBJS) $(LIBS)
	$(OBJCOPY) -O binary $(PRG).elf $(PRG).bin
	$(OBJDUMP) -S $(PRG).elf > $(PRG).lst
	$(SIZE) $(PRG).elf

flash: $(OBJS) Makefile $(PRG).bin
	stmflash $(PRG).bin

clean:
	rm -f $(OBJS) $(INTERMEDIATES)

