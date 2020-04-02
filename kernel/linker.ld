ENTRY(_start)
GROUP(../libc/libk.a)

SECTIONS
{
	. = 1M;

	.text : ALIGN(4K)
	{
		kernel_text = .;
		*(.multiboot)
		*(.text)
		*(.gnu.linkonce.t*)
		kernel_text_end = ALIGN(4K);

	}

	.rodata : ALIGN(4K)
	{
		kernel_rodata = .;
		*(.rodata)
		*(.gnu.linkonce.r*)
		. = ALIGN(8);
		__CTOR_LIST__ = .;
		*(.init_array)
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		__END_DTORS__ = .;
		kernel_rodata_end = ALIGN(4K);
	}

	.data : ALIGN(4K)
	{
		kernel_data = .;
		*(.data)
		kernel_data_end = ALIGN(4K);
	}

	.bss : ALIGN(4K)
	{
		kernel_bss = .;
		*(COMMON)
		*(.bss)
		kernel_bss_end = ALIGN(4K);
	}
	kernel_end = ALIGN(4K);
}
