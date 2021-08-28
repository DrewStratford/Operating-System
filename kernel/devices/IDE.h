#pragma once
#include <stdint.h>

namespace PCI {

const uint32_t CONFIG_ADDR = 0xCF8;
const uint32_t CONFIG_DATA = 0xCFC;

void check_devices(void);

}
