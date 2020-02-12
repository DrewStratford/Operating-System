#pragma once

#include <stdint.h>

namespace IO{

	void out8(uint16_t port, uint8_t byte);
	void out16(uint16_t port, uint16_t byte);
	void out32(uint16_t port, uint32_t byte);

	uint8_t in8(uint16_t port);
	uint16_t in16(uint16_t port);
	uint32_t in32(uint16_t port);
};

