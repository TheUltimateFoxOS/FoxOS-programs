OBJDIR = lib
BUILDDIR = bin

build:
	@echo "Building libc"
	@make -C libc

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
	@make -C calc

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

clean:
	@rm -r $(BUILDDIR)
	@rm -r $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

.PHONY: build
