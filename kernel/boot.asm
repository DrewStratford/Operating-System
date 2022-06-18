; Declare constants for the multiboot header.
MBALIGN  equ  1<<0              ; align loaded modules on page boundaries
MEMINFO  equ  1<<1              ; provide memory map
VIDINFO  equ  1<<2              ; use the video info from the header
; FLAGS    equ  MBALIGN | MEMINFO | VIDINFO ; this is the Multiboot 'flag' field
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot
 
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	;; empty space (due to this being an ELF file)
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
 
section .bss
align 4
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
 
; The entry point for the kernel.
; The GRUB info is stored in ebx.
section .text
global _start:
_start:
 
	; set up stack
	mov esp, stack_top
 
	push ebx		;push the GRUB multiboot info

	; Check for sse instructions
	mov eax, 0x1
	cpuid
	test edx, 1<<25
	jz .hang

	; We have sse so enable it
	mov eax, cr0
	and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
	or ax, 0x2			;set coprocessor monitoring  CR0.MP
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4, eax

	extern __CTOR_LIST__
	extern __CTOR_END__

	mov ebx, __CTOR_LIST__
	jmp .call_ctors_end
.call_ctors:
	call [ebx]
	add ebx, 4
.call_ctors_end:
	cmp ebx, __CTOR_END__
	jb .call_ctors

	extern kernel_main
	call kernel_main

 
	cli
.hang:	hlt
	jmp .hang
.end:
