#include <devices/Interrupts.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <Thread.h>

IDTEntry idt_table[256];
IDTR idtr(0,0);

InterruptCallback callbacks[256];
SystemCall system_calls[256];

extern "C" void interrupt_handler(Registers registers);
void system_call_handler(Registers& registers);

int32_t syscall_debug(Registers& registers){
	char** stack = (char**)registers.esp;
	char* message = stack[0];
	com1() << message;
	return 0;
}

#define INTERRUPT_ERRCODE(name, code)	\
extern "C" void name##_interrupt_entry();	\
asm (".section .text\n"	\
	""#name"_interrupt_entry:\n"	\
	"push $"#code"\n"	\
	"pusha\n"	\
	"push %fs\n"	\
	"push %gs\n"	\
	"push %es\n"	\
	"push %ds\n"	\
	"call interrupt_handler\n"	\
	"pop %ds\n"	\
	"pop %es\n"	\
	"pop %gs\n"	\
	"pop %fs\n"	\
	"popa\n"	\
	"add $8, %esp\n"	\
	"iret\n"	\
	);

#define INTERRUPT(name, code)	\
extern "C" void name##_interrupt_entry();	\
asm (".section .text\n"	\
	""#name"_interrupt_entry:\n"	\
	"push $0\n"	\
	"push $"#code"\n"	\
	"pusha\n"	\
	"push %fs\n"	\
	"push %gs\n"	\
	"push %es\n"	\
	"push %ds\n"	\
	"call interrupt_handler\n"	\
	"pop %ds\n"	\
	"pop %es\n"	\
	"pop %gs\n"	\
	"pop %fs\n"	\
	"popa\n"	\
	"add $8, %esp\n"	\
	"iret\n"	\
	);

INTERRUPT(irqff, 0xff);

INTERRUPT(irq00, 0x00);
INTERRUPT(irq01, 0x01);
INTERRUPT(irq02, 0x02);
INTERRUPT(irq03, 0x03);
INTERRUPT(irq04, 0x04);
INTERRUPT(irq05, 0x05);
INTERRUPT(irq06, 0x06);
INTERRUPT(irq07, 0x07);
INTERRUPT_ERRCODE(irq08, 0x08);
INTERRUPT(irq09, 0x09);
INTERRUPT_ERRCODE(irq0a, 0x0a);
INTERRUPT_ERRCODE(irq0b, 0x0b);
INTERRUPT_ERRCODE(irq0c, 0x0c);
INTERRUPT_ERRCODE(irq0d, 0x0d);
INTERRUPT_ERRCODE(irq0e, 0x0e);
INTERRUPT(irq0f, 0x0f);
INTERRUPT(irq10, 0x10);
INTERRUPT_ERRCODE(irq11, 0x11);
INTERRUPT(irq12, 0x12);
INTERRUPT(irq13, 0x13);

INTERRUPT(irq20, 0x20);
INTERRUPT(irq21, 0x21);
INTERRUPT(irq22, 0x22);
INTERRUPT(irq23, 0x23);
INTERRUPT(irq24, 0x24);
INTERRUPT(irq25, 0x25);
INTERRUPT(irq26, 0x26);
INTERRUPT(irq27, 0x27);
INTERRUPT(irq28, 0x28);
INTERRUPT(irq29, 0x29);
INTERRUPT(irq2a, 0x2a);
INTERRUPT(irq2b, 0x2b);
INTERRUPT(irq2c, 0x2c);
INTERRUPT(irq2d, 0x2d);
INTERRUPT(irq2e, 0x2e);
INTERRUPT(irq2f, 0x2f);

INTERRUPT(irq80, 0x80);

void initialize_interrupts(){
	com1().write_string("initializing IDT...\n");

	for(int i = 0; i < 256; i++){
		callbacks[i] = nullptr;
	}
	for(int i = 0; i < 256; i++){
		idt_table[i] = IDTEntry((uint32_t)irqff_interrupt_entry, 0x8E);
	}
	
	//reserved exceptions
	idt_table[0x00] = IDTEntry((uint32_t)irq00_interrupt_entry, 0x8E);
	idt_table[0x01] = IDTEntry((uint32_t)irq01_interrupt_entry, 0x8E);
	idt_table[0x02] = IDTEntry((uint32_t)irq02_interrupt_entry, 0x8E);
	idt_table[0x03] = IDTEntry((uint32_t)irq03_interrupt_entry, 0x8E);
	idt_table[0x04] = IDTEntry((uint32_t)irq04_interrupt_entry, 0x8E);
	idt_table[0x05] = IDTEntry((uint32_t)irq05_interrupt_entry, 0x8E);
	idt_table[0x06] = IDTEntry((uint32_t)irq06_interrupt_entry, 0x8E);
	idt_table[0x07] = IDTEntry((uint32_t)irq07_interrupt_entry, 0x8E);
	idt_table[0x08] = IDTEntry((uint32_t)irq08_interrupt_entry, 0x8E);
	idt_table[0x09] = IDTEntry((uint32_t)irq09_interrupt_entry, 0x8E);
	idt_table[0x0a] = IDTEntry((uint32_t)irq0a_interrupt_entry, 0x8E);
	idt_table[0x0b] = IDTEntry((uint32_t)irq0b_interrupt_entry, 0x8E);
	idt_table[0x0c] = IDTEntry((uint32_t)irq0c_interrupt_entry, 0x8E);
	idt_table[0x0d] = IDTEntry((uint32_t)irq0d_interrupt_entry, 0x8E);
	idt_table[0x0e] = IDTEntry((uint32_t)irq0e_interrupt_entry, 0x8E);
	idt_table[0x0f] = IDTEntry((uint32_t)irq0f_interrupt_entry, 0x8E);
	idt_table[0x10] = IDTEntry((uint32_t)irq10_interrupt_entry, 0x8E);
	idt_table[0x11] = IDTEntry((uint32_t)irq11_interrupt_entry, 0x8E);
	idt_table[0x12] = IDTEntry((uint32_t)irq12_interrupt_entry, 0x8E);
	idt_table[0x13] = IDTEntry((uint32_t)irq13_interrupt_entry, 0x8E);

	// IBM compatible interrupts
	idt_table[0x20] = IDTEntry((uint32_t)irq20_interrupt_entry, 0x8E);
	idt_table[0x21] = IDTEntry((uint32_t)irq21_interrupt_entry, 0x8E);
	idt_table[0x22] = IDTEntry((uint32_t)irq22_interrupt_entry, 0x8E);
	idt_table[0x23] = IDTEntry((uint32_t)irq23_interrupt_entry, 0x8E);
	idt_table[0x24] = IDTEntry((uint32_t)irq24_interrupt_entry, 0x8E);
	idt_table[0x25] = IDTEntry((uint32_t)irq25_interrupt_entry, 0x8E);
	idt_table[0x26] = IDTEntry((uint32_t)irq26_interrupt_entry, 0x8E);
	idt_table[0x27] = IDTEntry((uint32_t)irq27_interrupt_entry, 0x8E);
	idt_table[0x28] = IDTEntry((uint32_t)irq28_interrupt_entry, 0x8E);
	idt_table[0x29] = IDTEntry((uint32_t)irq29_interrupt_entry, 0x8E);
	idt_table[0x2a] = IDTEntry((uint32_t)irq2a_interrupt_entry, 0x8E);
	idt_table[0x2b] = IDTEntry((uint32_t)irq2b_interrupt_entry, 0x8E);
	idt_table[0x2c] = IDTEntry((uint32_t)irq2c_interrupt_entry, 0x8E);
	idt_table[0x2d] = IDTEntry((uint32_t)irq2d_interrupt_entry, 0x8E);
	idt_table[0x2e] = IDTEntry((uint32_t)irq2e_interrupt_entry, 0x8E);
	idt_table[0x2f] = IDTEntry((uint32_t)irq2f_interrupt_entry, 0x8E);

	// map 0x80 for userspace interrupts
	idt_table[0x80] = IDTEntry((uint32_t)irq80_interrupt_entry, 0x8E);
	callbacks[0x80] = system_call_handler;

	for(size_t i = 0; i < 256; i++)
		system_calls[i] = nullptr;

	register_system_call(syscall_debug, SC_debug);
	
	// remap the PIC
	IO::out8(0x20, 0x11); //restart both PICs
	IO::out8(0xA0, 0x11);

	IO::out8(0x21, 0x20);
	IO::out8(0xA1, 0x28);

	IO::out8(0x21, 0x04);
	IO::out8(0xA1, 0x02);

	IO::out8(0x21, 0x01);
	IO::out8(0xA1, 0x01);

	// load the idt
	idtr.limit = (sizeof(IDTEntry) * 256) - 1;
	idtr.base = (uint32_t)&idt_table;
	asm("lidt %0\n" ::"m"(idtr));
	com1().write_string("initialized IDT\n");

	//start the pit
	IO::out8(0x43, 0b00110100);

	//This timing is pretty arbitrary, it's only set
	// to be fast enough that the keyboard isn't laggy.
	IO::out8(0x40, 250);
	IO::out8(0x40, 10);
}

void register_interrupt_callback(InterruptCallback callback, size_t no){
	callbacks[no % 256] = callback;
}

void register_system_call(SystemCall syscall, size_t no){
	system_calls[no % 256] = syscall;
}

extern "C" void interrupt_handler(Registers registers){
	InterruptCallback handler = callbacks[registers.intr % 256];

	if(handler)
		handler(registers);
		
	IO::out8(0xA0, 0x20);
	IO::out8(0x20, 0x20);

	if(current_thread->should_die())
		current_thread->die();

	if(current_thread->get_remaining_ticks() <= 0)
		Thread::yield();

	current_thread->handle_signals();
}

void system_call_handler(Registers& registers){
	SystemCall system_call = system_calls[registers.eax % 256];

	if(system_call)
		registers.eax = system_call(registers);
}
