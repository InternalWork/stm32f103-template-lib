TARGETS = sender.elf receiver.elf

all: $(TARGETS)

CPU ?= STM32F103RB

CC = arm-none-eabi-gcc
CXX = arm-none-eabi-g++
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
SIZE = arm-none-eabi-size
LD = arm-none-eabi-ld
AS = arm-none-eabi-as

INC = -I. -I../.. -I../../..

CFLAGS = $(INC) -Os -g -mcpu=cortex-m3 -mthumb -nostartfiles -ffunction-sections -fdata-sections -DSTM32F10X_MD
CXXFLAGS = $(CFLAGS) -std=c++11 -fno-unwind-tables -fno-exceptions -Wl,--nostdlib -Wl,--gc-sections -L../.. -L../../../libaeabi-cortexm0 -T../../stm32f103xb.ld -laeabi-cortexm0

COMMON_SRCS = ../../vectors.cpp ../../utils.cpp

%.elf : %.cpp
	$(CXX) -o $@ $(CXXFLAGS) $($*_FLAGS) $(COMMON_SRCS) $<
	$(OBJDUMP) -S $@ > $*.lst
	$(SIZE) $@

clean:
	rm -f *.elf *.lst
