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

enum BUS{
	BUS_COMMAND = 0,
	DEV_SPECIFIC,
	BUS_STATUS = 2,
	DEV_SPECIFIC2,
	BUS_PRDT = 4
};

class [[gnu::packed]] PRD{
	public:
		uint32_t address {0};
		uint16_t count {0};
		uint16_t eot {0};

};

class ATA{
	private:
		uint32_t dma_base {0};
		volatile bool ready;

	public:
		static ATA* initialize(uint32_t busmaster);
		ATA(uint32_t busmaster);
		bool read(uint32_t lba, uint32_t size, uint8_t* buffer);
		bool read_dma(uint32_t lba, uint32_t size, uint8_t* buffer);
		bool dma_transfer(bool is_read, uint32_t lba, uint32_t size, uint8_t* buffer);
		bool write(uint32_t lba, uint32_t size, uint8_t* buffer);
		bool write_dma(uint32_t lba, uint32_t size, uint8_t* buffer);
		void setup_transfer(uint32_t lba, uint32_t size);
		void wait();
		void wait_async();
		void handle_interrupt();
};
