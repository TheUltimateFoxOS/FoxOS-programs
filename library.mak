OBJDIR = ../lib
BUILDDIR = ../bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CSRC = $(call rwildcard,./,*.c)
CPPSRC = $(call rwildcard,./,*.cpp)

OBJS = $(patsubst %.c, $(OBJDIR)/%_$(OUTPUT).o, $(CSRC))
OBJS += $(patsubst %.cpp, $(OBJDIR)/%_$(OUTPUT).o, $(CPPSRC))

TOOLCHAIN_BASE = /usr/local/foxos-x86_64_elf_gcc

CFLAGS = -O2 -mno-red-zone -ffreestanding -fno-exceptions -fpic -g -I ../libfoxos/include -Iinclude -I../libc/include -I../libtinf/include -I../libcfg/include -I../libfoxdb/include -nostdinc -fno-stack-protector -fdata-sections -ffunction-sections
CPPFLAGS = $(CFLAGS)
LDFLAGS = -r

CFLAGS += $(USER_CFLAGS)
CPPFLAGS += $(USER_CPPFLAGS)

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

ifeq (,$(wildcard $(TOOLCHAIN_BASE)/bin/foxos-ar))
	AR = ar
else
	AR = $(TOOLCHAIN_BASE)/bin/foxos-ar
endif

$(OUTPUT): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@.o $^
	@echo AR $(BUILDDIR)/$@.o
	@$(AR) rcs $(BUILDDIR)/$@ $(BUILDDIR)/$@.o
	@echo Succesfully build library $(OUTPUT)

$(OBJDIR)/%_$(OUTPUT).o: %.c
	@echo "CC $^ -> $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%_$(OUTPUT).o: %.cpp
	@echo "CC $^ -> $@"
	@mkdir -p $(@D)
	@$(CC) $(CPPFLAGS) -c -o $@ $^

.PHONY: build
