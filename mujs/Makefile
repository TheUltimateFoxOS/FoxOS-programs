OUTPUT = js.elf

all: mujs
	make $(OUTPUT)

mujs:
	git clone https://github.com/ccxvii/mujs.git
	cd mujs; git am < ../0001-some-fixes-for-foxos.patch; cd ..

include ../program.mak