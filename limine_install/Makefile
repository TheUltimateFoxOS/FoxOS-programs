OUTPUT = lminst.elf

all: ./include/limine-hdd.h $(OUTPUT)

./include/limine-hdd.h:
	curl -L https://github.com/limine-bootloader/limine/raw/9761d387a73a1d8ba517421ad3c7c6e6cda49626/limine-hdd.h -o $@

include ../program.mak