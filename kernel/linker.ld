ENTRY(_start)
GROUP(../libc/libk.a)

SECTIONS
{
	. = 1M;

	.text : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
		*(.gnu.linkonce.t*)

	}

	.rodata : ALIGN(4K)
	{

		*(.rodata)
		*(.gnu.linkonce.r*)
		. = ALIGN(8);
		__CTOR_LIST__ = .;
		*(.init_array)
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		__END_DTORS__ = .;
	}

	.data : ALIGN(4K)
	{
		*(.data)
	}

	.bss : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
	kernel_end = ALIGN(4K);
}
