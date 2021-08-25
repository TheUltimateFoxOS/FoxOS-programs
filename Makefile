OBJDIR = lib
BUILDDIR = bin

build:
	@echo "Building libc"
	@make -C libc

	@echo "Building programs"
	@make -C test
	@make -C terminal

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

clean:
	@rm -r $(BUILDDIR)
	@rm -r $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

.PHONY: build