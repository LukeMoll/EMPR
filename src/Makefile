BIN = empr.bin

MAIN ?= main

PKG = /opt/york/cs/net
PATH := $(PKG)/bin:$(PATH)

ARCH = arm-none-eabi
CC = $(ARCH)-gcc
OBJCOPY = $(ARCH)-objcopy

CMSIS = $(PKG)/lpc1700-cmsis-lite-2011.01.26-i686-1
CMSIS_LIB = $(CMSIS)/lib
CMSIS_INCLUDE = $(CMSIS)/include
CMSIS_OBJ = $(CMSIS)/lib/core_cm3.o $(CMSIS)/lib/system_LPC17xx.o \
	$(CMSIS)/lib/startup_LPC17xx.o
LDSCRIPT = $(CMSIS)/lib/ldscript_rom_gnu.ld

THUMB2_LIB = $(PKG)/sourcery-g++-lite-arm-eabi-2010.09.51-i686-1/$(ARCH)/lib/thumb2

LIBEMPR_SRC = ../libempr
LIBEMPR_LIB = $(LIBEMPR_SRC)/out/lib
LIBEMPR_INCLUDE = $(LIBEMPR_SRC)/out/include
LIBEMPR = $(LIBEMPR_LIB)/libempr.a

INCLUDES = -I$(CMSIS_INCLUDE) -I$(LIBEMPR_INCLUDE)
LIB_PATHS = -L$(THUMB2_LIB) -L$(CMSIS_LIB) -L$(LIBEMPR_LIB)

CFLAGS = -std=gnu99 -O0 -Wall $(INCLUDES) -D__thumb2__=1 -D__RAM_MODE__=0 \
	-fno-hosted -ffunction-sections -fdata-sections -gdwarf-2 -mcpu=cortex-m3 \
	-mthumb -mapcs-frame -msoft-float -mno-sched-prolog -mtune=cortex-m3 \
	-march=armv7-m -mfix-cortex-m3-ldrd

LDFLAGS = -static -mcpu=cortex-m3 -mthumb -mthumb-interwork $(LIB_PATHS) \
	-lempr -lDriversLPC17xxgnu -lc -lg -lstdc++ -lsupc++ -lgcc -lm -T$(LDSCRIPT)

OBJ = $(MAIN).o

.PHONY: all clean install

all: $(BIN)

$(BIN): $(LIBEMPR) $(OBJ)
	$(CC) -o $@ $(CMSIS_OBJ) $(OBJ) $(LDFLAGS) 
	$(OBJCOPY) -I elf32-little -O binary $@

clean:
	rm -f $(BIN)
	rm -f $(OBJ)

install: $(BIN)
	cp $< /media/$(USER)/MBED
	sync

$(LIBEMPR): $(wildcard $(LIBEMPR_SRC)/*.c $(LIBEMPR_SRC)/*.h)
	make -C $(LIBEMPR_SRC) all
