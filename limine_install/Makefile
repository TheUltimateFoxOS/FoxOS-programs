OUTPUT = lminst.elf

all: ./include/limine-hdd.h $(OUTPUT)

./include/limine-hdd.h:
	curl -L https://github.com/limine-bootloader/limine/raw/v3.0-branch-binary/limine-hdd.h -o $@

include ../program.mak