OBJDIR = lib
BUILDDIR = bin

build:
	@echo "Building libaries"
	@make -C libfoxos
	@make -C libc
	@make -C libtinf
	@make -C libcfg

	@echo "Building programs"
	@make -C test
	@make -C window_test
	@make -C terminal
	@make -C init
	@make -C cat
	@make -C echo
	@make -C env
	@make -C ls
	@make -C rm
	@make -C mkdir
	@make -C date
	@make -C install
	@make -C calc
	@make -C ping
	@make -C shutdown
	@make -C du
	@make -C keymap_tool
	@make -C cp
	@make -C limine_install
	@make -C lsfs
	@make -C clear
	@make -C lolcat
	@make -C matrix
	@make -C tasks
	@make -C nyanmbr
	@make -C saf_extract
	@make -C saf_make
	@make -C foxe
	@make -C touch
	@make -C imgview
	@make -C tic_tac_toe
	@make -C play
	@make -C rmdir
	@make -C reboot
	@make -C file
	@make -C paint
	@make -C lsmod
	@make -C if
	@make -C free
	@make -C lscpu
	@make -C smbios32
	@make -C foxde
	@make -C insmod
	@make -C nslookup
	@make -C gunzip

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

clean:
	@rm -r $(BUILDDIR)
	@rm -r $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

.PHONY: build
