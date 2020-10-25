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
  }

  .data : ALIGN(4K){
    *(.data)
  }

  .bss : ALIGN(4K) {
    *(.bss)
  }

_BINARY_END_ = . ;
}

