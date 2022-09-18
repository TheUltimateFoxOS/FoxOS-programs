OBJDIR = ../lib
BUILDDIR = ../bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CSRC = $(call rwildcard,./,*.c)
CPPSRC = $(call rwildcard,./,*.cpp)

OBJS = $(patsubst %.c, $(OBJDIR)/%_$(OUTPUT).o, $(CSRC)) $(C_OBJS)
OBJS += $(patsubst %.cpp, $(OBJDIR)/%_$(OUTPUT).o, $(CPPSRC)) $(CPP_OBJS)

TOOLCHAIN_BASE = /usr/local/foxos-x86_64_elf_gcc

CFLAGS = -O2 -mno-red-zone -ffreestanding -fno-exceptions -fno-stack-protector -fpic -g -I ../libfoxos/include -I ../libc/include -I ../libtinf/include -I ../libcfg/include -I ../libfoxdb/include -Iinclude  -fdata-sections -ffunction-sections
CPPFLAGS = $(CFLAGS)
LDFLAGS = -pic $(BUILDDIR)/libfoxos.a.o $(BUILDDIR)/libc.a.o $(BUILDDIR)/libtinf.a.o $(BUILDDIR)/libcfg.a.o $(BUILDDIR)/libfoxdb.a.o --gc-sections

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

$(OUTPUT): $(OBJS)
	@echo LD $^
	@$(LD) $(LDFLAGS) -o $(BUILDDIR)/$@ $^
	@echo Succesfully build program $(OUTPUT)

$(OBJDIR)/%_$(OUTPUT).o: %.c
	@echo "CC $^ -> $@"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $^

$(OBJDIR)/%_$(OUTPUT).o: %.cpp
	@echo "CC $^ -> $@"
	@mkdir -p $(@D)
	@$(CC) $(CPPFLAGS) -c -o $@ $^

.PHONY: build
