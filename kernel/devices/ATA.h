#pragma once

#include <stdint.h>

const uint32_t ATA_BASE = 0x1f0;
const uint32_t ATA_CONTROL = 0x3f6;

enum ATA_IO{
	DATA = 0,
	ERROR = 1,
	FEATURES = 1,
	SECTOR_COUNT,
	CYLINDER_LOW,
	CYLINDER_MID,
	CYLINDER_HI,
	DRIVE,
	STATUS = 7,
	COMMAND = 7,
};

enum ATA_CONTROL{
	ASTATUS = 0,
	CONTROL = 0,
	DRIVEADDR = 1,
};


class ATA{
	public:
		ATA();
		bool read(uint32_t lba, uint32_t size, uint8_t* buffer);
		bool write(uint32_t lba, uint32_t size, uint8_t* buffer);
		void setup_transfer(uint32_t lba, uint32_t size);
		void wait();
		void wait_async();
};
