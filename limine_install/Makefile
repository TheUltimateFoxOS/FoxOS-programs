OUTPUT = lminst.elf

all: ./include/limine-hdd.h $(OUTPUT)

LIMINE_BRANCH = v4.x-branch-binary
# LIMINE_BRANCH = v3.0-branch-binary

./include/limine-hdd.h:
	echo "Downloading $(LIMINE_BRANCH) limine-hdd.h"
	curl -L https://github.com/limine-bootloader/limine/raw/$(LIMINE_BRANCH)/limine-hdd.h -o $@

include ../program.mak