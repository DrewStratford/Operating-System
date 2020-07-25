#pragma once

#include <stdint.h>

struct Time {
	int32_t year { 0 };
	int32_t month { 0 };
	int32_t day { 0 };
	int32_t hour { 0 };
	int32_t minute { 0 };
	int32_t second { 0 };
};

Time from_timecode(int32_t timecode);

int32_t make_timecode(int32_t y, int32_t m, int32_t d, int32_t h, int32_t min, int32_t s);

