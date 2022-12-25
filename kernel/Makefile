AS=nasm
AFLAGS=-f elf32
CC=gcc
CFLAGS=-std=c11 -Wall -O0 -pedantic -g -fvar-tracking -m32 \
	-ffreestanding -fno-pie -fno-pic -fno-builtin \
	-nostdlib -Wno-multichar -nostartfiles \
	-I. -I./drivers -I./common -I./cpu
LDFLAGS=
BACKUPS=$(shell find . -iname "*.bak")
OBJECTS=entry.o cpu/int.o cpu/gen_interrupt.o cpu/gdt_flush.o
OBJECTS+=\
	kernel.o shell.o common/io.o common/helper.o \
	common/util.o drivers/vga.o drivers/keyboard.o \
	cpu/idt.o cpu/isr.o	cpu/system.o cpu/ports.o \
	cpu/regs.o cpu/task.o cpu/timer.o cpu/gdt.o

TARGET=kernel
TARGET_BIN=$(TARGET).bin
TARGET_ELF=$(TARGET).elf

.PHONY: all clean distclean
all: $(TARGET_BIN)

%.o: %.asm
	@echo " AS $<"
	@$(AS) $(AFLAGS) -o $@ $<

%.o: %.c
	@echo " CC $<"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET_BIN): $(TARGET_ELF)
	@echo " ELF2BIN $@"
	@objcopy -O binary $(TARGET_ELF) $@

$(TARGET_ELF): $(OBJECTS)
	@echo " LD $@"
	@$(LD) $(LDFLAGS) -Map=$(TARGET).map -Tlink.ld -o $@ $(OBJECTS)

clean:
	rm -f $(OBJECTS) $(TARGET_BIN) *.elf

distclean: clean
ifneq ($(BACKUPS),)
	rm -f $(BACKUPS)
endif
