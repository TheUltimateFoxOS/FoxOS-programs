OBJDIR = lib
BUILDDIR = bin

build:
	@echo "Building libaries"
	@make -C libc
	@make -C libterm
	@make -C libgraphics

	@echo "Building programs"
	@make -C test
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

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

clean:
	@rm -r $(BUILDDIR)
	@rm -r $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

.PHONY: build
