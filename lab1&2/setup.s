!
!	setup.s		(C) 1991 Linus Torvalds
!
! setup.s is responsible for getting the system data from the BIOS,
! and putting them into the appropriate places in system memory.
! both setup.s and system has been loaded by the bootblock.
!
! This code asks the bios for memory/disk/other parameters, and
! puts them in a "safe" place: 0x90000-0x901FF, ie where the
! boot-block used to be. It is then up to the protected mode
! system to read them from there before the area is overwritten
! for buffer-blocks.
!

! NOTE! These had better be the same as in bootsect.s!

INITSEG  = 0x9000	! we move boot here - out of the way
SYSSEG   = 0x1000	! system loaded at 0x10000 (65536).
SETUPSEG = 0x9020	! this is the current segment

.globl begtext, begdata, begbss, endtext, enddata, endbss
.text
begtext:
.data
begdata:
.bss
begbss:
.text

entry start
start:

! Print some inane message

	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#25
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	ax,cs
	mov	es,ax
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

! init ss:sp
	mov ax,#INITSEG
	mov ss,ax
	mov sp,#0xFF00

! ok, the read went well so we get current cursor position and save it for
! posterity.

	mov	ax,#INITSEG	! this is done in bootsect already, but...
	mov	ds,ax
	mov	ah,#0x03	! read cursor pos
	xor	bh,bh
	int	0x10		! save it in known place, con_init fetches
	mov	[0],dx		! it from 0x90000.
! Get memory size (extended mem, kB)

	mov	ah,#0x88
	int	0x15
	mov	[2],ax



! Get hd0 data

	mov	ax,#0x0000
	mov	ds,ax
	lds	si,[4*0x41]
	mov	ax,#INITSEG
	mov	es,ax
	mov	di,#0x0004
	mov	cx,#0x10
	rep
	movsb

! Be Ready to Print
	mov ax,cs
	mov es,ax
	mov ax,#INITSEG
	mov ds,ax

! Cursor Position
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#18
	mov bx,#0x0007
	mov bp,#msg_cursor
	mov ax,#0x1301
	int 0x10
	mov dx,[0]
	call	print_hex
! Memory Size
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#14
	mov bx,#0x0007
	mov bp,#msg_memory
	mov ax,#0x1301
	int 0x10
	mov dx,[2]
	call print_hex

!Add KB
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#2
	mov bx,#0x0007
	mov bp,#msg_kb
	mov ax,#0x1301
	int 0x10
! Cyles
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#7
	mov bx,#0x0007
	mov bp,#msg_cyles
	mov ax,#0x1301
	int 0x10
	mov dx,[4]
	call print_hex
! Heads
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#8
	mov bx,#0x0007
	mov bp,#msg_heads
	mov ax,#0x1301
	int 0x10
	mov dx,[6]
	call print_hex
!Secotrs
	mov ah,#0x03
	xor bh,bh
	int 0x10
	mov cx,#10
	mov bx,#0x0007
	mov bp,#msg_sectors
	mov ax,#0x1301
	int 0x10
	mov dx,[12]
	call print_hex

inf_loop:
	jmp inf_loop

print_hex:
	mov cx,#4
print_digit:
	rol dx,#4
	mov ax,#0x0e0f
	and al,dl
	add al,#0x30
	cmp al,#0x3a
	jl outp
	add al,#0x07
outp:
	int 0x10
	loop print_digit
	ret



msg1:
	.byte 13,10
	.ascii "Now we are in SETUP"
	.byte 13,10,13,10

msg_cursor:
    .byte 13,10
    .ascii "Cursor position:"
msg_memory:
    .byte 13,10
    .ascii "Memory Size:"
msg_kb:
    .ascii "KB"
msg_cyles:
    .byte 13,10
    .ascii "Cyls:"
msg_heads:
    .byte 13,10
    .ascii "Heads:"
msg_sectors:
    .byte 13,10
    .ascii "Sectors:"
boot_flag:
	.word 0xAA55
.text
endtext:
.data
enddata:
.bss
endbss:
