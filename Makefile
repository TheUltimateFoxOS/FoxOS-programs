OBJDIR = lib
BUILDDIR = bin

build:
	@echo "Building libaries"
	@make -C libc
	@make -C libterm

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
	@make -C mujs
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

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

clean:
	@rm -r $(BUILDDIR)
	@rm -r $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

.PHONY: build
