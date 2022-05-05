OBJDIR = ../lib
BUILDDIR = ../bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CPPSRC = $(call rwildcard,./,*.c)
OBJS = $(patsubst %.c, $(OBJDIR)/%_$(OUTPUT).o, $(CPPSRC)) $(C_OBJS)

TOOLCHAIN_BASE = /usr/local/foxos-x86_64_elf_gcc

CFLAGS = -mno-red-zone -ffreestanding -fno-stack-protector -fpic -g -I ../libc/include -I ../libterm/include -I ../libgraphics/include -Iinclude  -fdata-sections -ffunction-sections
LDFLAGS = -pic $(BUILDDIR)/libc.a $(BUILDDIR)/libterm.a.o $(BUILDDIR)/libgraphics.a.o --gc-sections

CFLAGS += $(USER_CFLAGS)

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

$(OUTPUT): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@ $^
	@echo Succesfully build program $(OUTPUT)

$(OBJDIR)/%_$(OUTPUT).o: %.c
	@echo "CC $^ -> $@"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: build
