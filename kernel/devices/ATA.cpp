#include <devices/ATA.h>
#include <devices/IO.h>
#include <devices/Serial.h>

ATA::ATA(){
	//allow interrupts
	IO::out8(ATA_CONTROL, 0x0);
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

	com1() << "test: " << buffer[1] << "\n\n";
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
