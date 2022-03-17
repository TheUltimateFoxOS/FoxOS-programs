[bits 16]
[org 0x7c00]

_start:
	push cs
	pop ds
	push cs
	pop es
	cld

	mov cx, 25
	mov di, starlist
.loop_0:
	rdtsc
	xchg si, ax
	xor ax, si
	rol si, 7
	stosw
	loop .loop_0

	mov ax, 0x13
	int 0x10

	push 0xa000
	pop es

	push 4
	push es

	mov bp, 320 * 2

nyan:
	xor di, di
	mov ax, 126
	xor cx, cx
	dec cx
	rep stosb

	mov di, 64 * 320

	mov cl, 5
rainbow:
	push di
	add di, bp
	neg bp

	push cx
	mov cl, 5
	mov al, 40
.loop_0:
	mov bx, 24
	mov dx, 12
	call rect
	add al, 4
	loop .loop_0

	pop cx

	pop di
	add di, 24
	loop rainbow

	add di, bp
	push di

	mov cl, 25
	mov bx, starlist
starloop:
	sub [bx], word 4
	mov di, [bx]
	cmp [bx], byte 0x28
	ja .j_2
	mov si, star1
	jp .j_1
	mov si, star2

.j_1:
	push bx
	call xsquares
	pop bx
.j_2:
	inc bx
	inc bx
	loop starloop

tail:
	mov cl, 3
	mov di, 92*320+100
.loop_0:
	mov bx, 12
	xor ax, ax
	call square
	add di, 320*-8+4
	mov al, 25
	mov dl, 4
	call rect
	add di, 320*-8+-16
	sub di, bp
	sub di, bp
	loop .loop_0

	pop di
	mov si, data

sammish:
	call xslantedrect
	call double_xslantedrect

	pop ax
	pop dx
	neg ax
	jns .hop
	neg bp
	neg dx
.hop:
	neg bp
	push dx
	push ax
	sub di, dx

catface:

	add di, 320*24+44

	call double_xslantedrect

	mov cl, 5
.loop_0:
	call xsquares
	loop .loop_0

_feet:
	add di, 320 * 8 - 72
	call foot
	add di, 20
	call foot
	add di, 36
	call foot
	add di, 24
	call foot

	cwd
	inc cx
	mov ah, 0x86
	int 0x15

	mov dx, 0x3da
	wait_for_retrace_end:
	in ax, dx
	test al, 8
	jz wait_for_retrace_end
	wait_for_retrace_start:
	in ax, dx
	test al, 8
	jnz wait_for_retrace_start
	jmp nyan


square:
	mov dx, bx
rect:
	push cx
.loop_0:
	push di
	mov cx, bx
	rep stosb
	pop di
	add di, 320
	dec dx
	jne .loop_0
	pop cx
some_ret:
	ret

xsquares:
	lodsb
	xchg bx, ax
	lodsb
.loop_0:
	xchg dx, ax
	lodsb
	cmp al, 4
	je some_ret
	call load_coord_jump
	xchg dx, ax
	call square
	jmp .loop_0

double_xslantedrect:
	call xslantedrect

xslantedrect:
	lodsb
	call load_coord_jump
	push di
	xor ax, ax
	lodsb
	xchg cx, ax
	lodsb
	xchg bx, ax
	lodsb
	xchg dx, ax
	lodsb
.loop_0:
	pusha
	call rect
	popa
	sub dl, 8
	add bl, 8
	add di, 320*4-4
	loop .loop_0
	pop di
	ret

load_coord_jump: ; byte -> ( x:[-10..21], y:[-4..3] ) -> x*4 + y*1280
	xor ah, ah
	shl ax, 2
	add di, ax
	shr ax, 7
	imul ax, ax, 1152
	add ax, -5160
	add di, ax
	ret

foot:
	pusha
	call double_xslantedrect
	popa
	ret

data:
	db 106
	db 3, 72, 72, 0

	db 170
	db 2, 72, 64, 89

	db 172
	db 3, 56, 56, 60

	db 138
	db 4, 40, 40, 0

	db 170
	db 3, 40, 32, 25

	db 16, 25
	db 40, 20, 4

	db 8, 65
	db 225, 85, 4

	db 8,0
	db 1, 81, 4

	db 4, 15
	db 67, 113, 4

	db 4, 0
	db 32, 74, 74, 74, 75, 107, 139, 138, 113, 74, 75, 107, 139, 138, 138, 138, 197, 166, 138, 107, 107, 107, 107, 107, 107, 74, 103, 4

feet:
	db 138
	db 2, 8, 12, 0

	db 170
	db 1, 8, 4, 25

star1:
	db 4, 15
	db 138, 10, 138, 167, 107, 110, 107, 167, 138

star2:
	db 4, 15
	db 42, 140, 171, 169, 136, 72, 41, 43, 4

starlist:

; Make bootable
times 510-($-$$) db 0
dw 0xaa55
