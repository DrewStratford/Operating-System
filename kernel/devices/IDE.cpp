#include <devices/IDE.h>
#include <devices/IO.h>
#include <devices/Serial.h>

namespace PCI{

char *pci_classes[] = {
	"unclassified", "mass storage controller", "network controller",
	"display controller", "multimedia controller", "memory controller",
	"bridge device", "simple communication controller", "base system peripheral",
	"input device controller", "docking station", "processor", "serial bus controller",
	"wireless controller", "intelligent controller", "satellite communication controller",
	"encryption controller", "signal processing controller", "processing acccelerator",
	"non essential instrumentation"
};


uint32_t read32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset){
	uint32_t bus32    = (uint32_t)bus;
	uint32_t device32 = (uint32_t)device;
	uint32_t func32   = (uint32_t)func;

	uint32_t address =
			(bus32 << 16) | (device32 << 11) |
			(func32 << 8) | (offset & 0xfc) |
			((uint32_t)0x80000000);

	IO::out32(PCI::CONFIG_ADDR, address);

	// hacky wait a bit for some data.
	for(int i = 0; i < 10; i++) {
		IO::in32(PCI::CONFIG_DATA);
	}
	return IO::in32(PCI::CONFIG_DATA);
}

void write32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t data){
	uint32_t bus32    = (uint32_t)bus;
	uint32_t device32 = (uint32_t)device;
	uint32_t func32   = (uint32_t)func;

	uint32_t address =
			(bus32 << 16) | (device32 << 11) |
			(func32 << 8) | (offset & 0xfc) |
			((uint32_t)0x80000000);

	IO::out32(PCI::CONFIG_ADDR, address);
	IO::out32(PCI::CONFIG_DATA, data);
}

// Finds the IO Registers for the busmaster dma device
// returns 0 if nothing was found.
uint32_t find_busmaster(){
	for(int bus = 0; bus < 256; bus++){
		for(int device = 0; device < 32; device++){
			for(int function = 0; function < 8; function++){
				uint32_t id = read32(bus,device, function, 0);
				if(id >> 16 == 0xFFFF && function == 0)
					break;
				if(function == 0 && !(read32(bus, device, 0, 0x0c) & 0x800000))
					break;

				uint32_t class_info = read32(bus, device, function, 0x08);
				// A Bus master dma has a class, subclass that == 1
				// and prog_if with the 7th bit set.
				// It should have 16 registers stored in BAR4
				bool class_subclass = (class_info >> 16) == 0x0101;
				bool prog_if        = class_info & 0x8000;
				if(class_subclass && prog_if){
					// We need to ensure that bus master is enabled in the command register.
					// Otherwise DMA won't work.
					uint32_t command_reg = read32(bus,device,function,0x4) | 0x4;
					write32(bus, device, function, 0x04, command_reg);
					// BAR4, since it's IO space we zero out the lsb.
					return read32(bus,device,function,0x20) ^ 1;
				}

			}
		}
	}
	return 0;
}

void check_devices(void){
	com1() << "\nlooking up pci devices\n===============\n";
	for(int bus = 0; bus < 256; bus++){
		for(int device = 0; device < 32; device++){
			for(int function = 0; function < 8; function++){
				uint32_t id = read32(bus,device, function, 0);
				if(id >> 16 == 0xFFFF && function == 0)
					break;
				if(function == 0 && !(read32(bus, device, 0, 0x0c) & 0x800000))
					break;

				uint32_t class_info = read32(bus, device, function, 0x08);
				uint8_t class_      = (class_info >> 24) & 0xF;
				uint8_t subclass   = (class_info >> 16) & 0xF;
				uint8_t prog_if    = (class_info >> 8) & 0xF;

				com1() << "found device:\n"
				<< ">   vendor=" << (void*)(id&0xFFFF) << ", id=" << (void*)(id>>16) << "\n"
				<< ">   bus=" << bus << ", device=" << device << ", function=" << function << "\n"
				<< ">   class= " << pci_classes[class_] << ", subclass=" << subclass <<", prog_if=" << prog_if << "\n";

				uint32_t int_info = read32(bus, device, function, 0x3c);
				uint8_t int_pin = int_info >> 8;
				uint8_t int_line = int_info;
				com1() << ">   int_pin=" << int_pin << ", int_line=" << int_line<< "\n";

				uint32_t bar0 = read32(bus,device,function,0x10);
				uint32_t bar1 = read32(bus,device,function,0x14);
				uint32_t bar2 = read32(bus,device,function,0x18);
				uint32_t bar3 = read32(bus,device,function,0x1c);
				uint32_t bar4 = read32(bus,device,function,0x20);

				com1() << "bar0=" << (void*)bar0
					<< ", bar1=" << (void*)bar1
					<< ", bar2=" << (void*)bar2
					<< ", bar3=" << (void*)bar3
					<< ", bar4=" << (void*)bar4 << "\n";
			}
		}
	}
}
}
