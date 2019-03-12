NAME = libempr
OUT = out
LIB = $(OUT)/lib/$(NAME).a
INCLUDE = $(OUT)/include/$(NAME)

PKG = /opt/york/cs/net
PATH := $(PKG)/bin:$(PATH)
CMSIS_INCLUDE = $(PKG)/lpc1700-cmsis-lite-2011.01.26-i686-1/include

ARCH = arm-none-eabi
CC = $(ARCH)-gcc
AR = $(ARCH)-ar

CFLAGS = -std=gnu99 -O0 -Wall -I$(CMSIS_INCLUDE) -D__thumb2__=1 \
	-D__RAM_MODE__=0 -fno-hosted -ffunction-sections -fdata-sections -gdwarf-2 \
   	-mcpu=cortex-m3 -mthumb -mapcs-frame -msoft-float -mno-sched-prolog \
	-mtune=cortex-m3 -march=armv7-m -mfix-cortex-m3-ldrd

OBJ = $(patsubst %.c,%.o,$(wildcard *.c))
HEADERS = $(wildcard *.h)

.PHONY: all clean

all: $(LIB)

clean:
	rm -rf $(OUT)
	rm -f $(OBJ)

$(LIB): $(OBJ) $(HEADERS)
	mkdir -p $(dir $@)
	$(AR) rcs $@ $(OBJ)
	mkdir -p $(INCLUDE)
	cp $(HEADERS) $(INCLUDE)
