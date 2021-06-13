OBJDIR = ../lib
BUILDDIR = ../bin

TOOLCHAIN_BASE = /usr/local/foxos-x86_64_elf_gcc

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

build:
	gcc test.c -o $(OBJDIR)/test.o -mno-red-zone -ffreestanding -fpic -c
	ld $(OBJDIR)/test.o -o $(BUILDDIR)/test.elf -pic

.PHONY: build