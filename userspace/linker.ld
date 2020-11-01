ENTRY(_start)
OUTPUT_FORMAT(binary)
OUTPUT(a.out)
STARTUP(crt0.o)
INPUT(../libc/libc.a)

base = 0x40000000;
SECTIONS
{
  .text (base) :  {
    *(.text)
	*.(gnu.linkonce.t*)
  }

  .rodata : ALIGN(4K) {
	*(.rodata)
	*(.gnu.linkonce.r*)
	. = ALIGN(8);
	__CTOR_LIST__ = .;
	*(.init_array)
	__CTOR_END__ = .;
	__DTOR_LIST__ = .;
	__END_DTORS__ = .;
  }

  .data : ALIGN(4K){
    *(.data)
	*(COMMON)
    *(.bss)
  }

  .bss : ALIGN(4K) {
	*(COMMON)
    *(.bss)
  }
  . = . + SIZEOF(.bss);

_BINARY_END_ = . ;
}

