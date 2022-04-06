OUTPUT = nyanmbr.elf

all: nyan.bin $(OUTPUT)

nyan.bin: nyan.asm
	$(ASM) -f bin -o $@ $^

C_OBJS = $(OBJDIR)/nyanmbr_binfile.o
include ../program.mak

$(OBJDIR)/nyanmbr_binfile.o: binfile.asm
	@echo "ASM $^ -> $@"
	@mkdir -p $(@D)
	@$(ASM) -f elf64 -o $@ $^

.PHONY: build
