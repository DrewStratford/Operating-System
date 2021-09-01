#include <devices/ATA.h>
#include <devices/IO.h>
#include <devices/Interrupts.h>
#include <devices/Serial.h>
#include <memory/Paging.h>

alignas (4) PRD pdrt[8];
uint32_t dma_base_ = 0;

ATA* ata = nullptr;

void dma_callback(Registers&){
	ata->handle_interrupt();
}

ATA* ATA::initialize(uint32_t busmaster){
	return (ata = new ATA(busmaster));
}

ATA::ATA(uint32_t busmaster) : dma_base(busmaster){
	register_interrupt_callback(dma_callback, 0x2e);
	//allow interrupts
	IO::out8(ATA_CONTROL, 0x0);
	dma_base_ = dma_base;
}

void ATA::handle_interrupt(){
	uint8_t status = IO::in8(dma_base_+BUS_STATUS);
	uint8_t dstatus = IO::in8(ATA_BASE+STATUS);
	IO::out8(dma_base+BUS_STATUS, 0x64);
	IO::out8(dma_base+BUS_COMMAND, 0x0);
	com1() << "in callback " << (void*)status << ", " << (void*)dstatus << "\n" ;
	ready = true;

}

void ATA::wait(){
	while(IO::in8(ATA_BASE+STATUS) & 0b10000000){
	}
}

void ATA::wait_async(){
	while(IO::in8(ATA_CONTROL+ASTATUS) & 0b10000000){
	}
}

void ATA::setup_transfer(uint32_t lba, uint32_t size){
	IO::out8(ATA_BASE+DRIVE, 0xE0);
	IO::out8(ATA_BASE+SECTOR_COUNT, size / 512);
	IO::out8(ATA_BASE+CYLINDER_LOW, lba);
	IO::out8(ATA_BASE+CYLINDER_MID, lba >> 8);
	IO::out8(ATA_BASE+CYLINDER_HI, lba >> 16);
}

bool ATA::read(uint32_t lba, uint32_t size, uint8_t* buffer){
	if(size % 512 != 0)
		return false;


	uint16_t* buf = (uint16_t*)buffer;
	setup_transfer(lba, size);
	IO::out8(ATA_BASE+COMMAND, 0x20);
	wait();
	for(int j = 0; j < size/512; j++){
		for(int i = 0; i < 512/2; i++, buf++){
			*buf = IO::in16(ATA_BASE+DATA);
		}
		wait();
	}
	return true;
}

bool ATA::write(uint32_t lba, uint32_t size, uint8_t* buffer){
	if(size % 512 != 0)
		return false;

	uint16_t* buf = (uint16_t*)buffer;
	setup_transfer(lba, size);
	IO::out8(ATA_BASE+COMMAND, 0x30);
	wait();
	for(int j = 0; j < size/512; j++){
		for(int i = 0; i < 512/2; i++, buf++){
			wait_async();
			IO::out16(ATA_BASE+DATA, *buf);
		}
		wait();
	}

	return true;
}

bool ATA::dma_transfer(bool is_read, uint32_t lba, uint32_t size, uint8_t* buffer){
	if(size % 512 != 0)
		return false;

	ready = false;

	// Set up PRDT
	pdrt[0].address = v_to_p((uintptr_t)buffer);
	pdrt[0].count = (uint16_t)size;
	pdrt[0].eot = 0x8000;

	// STOP
	IO::out8(dma_base+BUS_COMMAND, 0);
	// Load PRDT
	IO::out32(dma_base+BUS_PRDT, v_to_p((uintptr_t)&pdrt));
	IO::out8(dma_base+BUS_STATUS, IO::in8(dma_base+BUS_STATUS) | 0x60);

	// Set up disk
	setup_transfer(lba, size);
	uint8_t ata_cmd = is_read ? 0xc8 : 0xca;
	IO::out8(ATA_BASE+COMMAND, ata_cmd);

	// Start the transfer
	uint8_t dma_cmd = is_read ? 0x00 : 0x08;
	IO::out8(dma_base+BUS_COMMAND,  dma_cmd | 0x1);

	// Janky spinloop
	while(!ready){ }

	com1 () << "buffer[10] = " << buffer[10] << "\n";
	com1 () << "buffer[513] = " << buffer[513] << "\n";
	com1() << "status: " << IO::in8(dma_base+BUS_STATUS) << "\n";
	com1() << "status: " << IO::in8(ATA_BASE+STATUS) << "\n";
	return true;
}

bool ATA::read_dma(uint32_t lba, uint32_t size, uint8_t* buffer){
	return dma_transfer(true, lba, size, buffer);
}

bool ATA::write_dma(uint32_t lba, uint32_t size, uint8_t* buffer){
	return dma_transfer(false, lba, size, buffer);
}
