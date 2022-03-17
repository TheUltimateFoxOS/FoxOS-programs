OBJDIR = ../lib
BUILDDIR = ../bin

OUTPUT = nyanmbr.elf

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CPPSRC = $(call rwildcard,./,*.c)
OBJS = $(patsubst %.c, $(OBJDIR)/%_$(OUTPUT).o, $(CPPSRC)) $(OBJDIR)/binfile.o


TOOLCHAIN_BASE = /usr/local/foxos-x86_64_elf_gcc

CFLAGS = -mno-red-zone -ffreestanding -fpic -fno-stack-protector -g -I ../libc/include -Iinclude
LDFLAGS = -pic $(BUILDDIR)/libc.a

ifeq (,$(wildcard $(TOOLCHAIN_BASE)/bin/foxos-gcc))
	CC = gcc
else
	CC = $(TOOLCHAIN_BASE)/bin/foxos-gcc
endif

ifeq (,$(wildcard $(TOOLCHAIN_BASE)/bin/foxos-nasm))
	ASM = nasm
else
	ASM = $(TOOLCHAIN_BASE)/bin/foxos-nasm
endif

ifeq (,$(wildcard $(TOOLCHAIN_BASE)/bin/foxos-gcc))
	LD = ld
else
	LD = $(TOOLCHAIN_BASE)/bin/foxos-ld
endif

all: nyan.bin $(OUTPUT)

nyan.bin: nyan.asm
	$(ASM) -f bin -o $@ $^


$(OUTPUT): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@ $^

$(OBJDIR)/%_$(OUTPUT).o: %.c
	@echo "CC $^ -> $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/binfile.o: binfile.asm
	@echo "ASM $^ -> $@"
	@mkdir -p $(@D)
	@$(ASM) -f elf64 -o $@ $^

.PHONY: build
