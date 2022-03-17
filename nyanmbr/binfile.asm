%macro inc_bin 2
	SECTION .rodata
	GLOBAL %1
	GLOBAL %1_size
%1:
	incbin %2
	db 0
	%1_size: dq %1_size - %1 -1
%endmacro

inc_bin nyan_mbr, "nyan.bin"