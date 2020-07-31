#include <devices/Clock.h>
#include <devices/IO.h>
#include <devices/Serial.h>
#include <devices/Interrupts.h>

#include <Time.h>

namespace Clock{

constexpr uint8_t cmos_port = 0x70;
constexpr uint8_t cmos_data = 0x71;

constexpr uint8_t cmos_year = 0x09;
constexpr uint8_t cmos_month = 0x08;
constexpr uint8_t cmos_day = 0x07;
constexpr uint8_t cmos_hour = 0x04;
constexpr uint8_t cmos_minute = 0x02;
constexpr uint8_t cmos_second = 0x00;

constexpr uint8_t cmos_status = 0x0B;


static uint8_t read_cmos(uint8_t reg){
	IO::out8(cmos_port, reg);
	return IO::in8(cmos_data);
}

static uint8_t bcd(uint8_t byte){
	uint8_t high = byte >> 4;
	uint8_t low = byte & 0xF;
	return (high * 10) + low;
}


int32_t current(){
	uint8_t year = read_cmos(cmos_year);
	uint8_t month = read_cmos(cmos_month);
	uint8_t day = read_cmos(cmos_day);
	uint8_t hour = read_cmos(cmos_hour);
	uint8_t minute = read_cmos(cmos_minute);
	uint8_t second = read_cmos(cmos_second);

	uint8_t status = read_cmos(cmos_status);

	// Convert from bcd.
	if(!(status & 0x04)){
		year = bcd(year);
		month = bcd(month);
		day = bcd(day);
		hour = bcd(hour) | (hour & 0x80);
		minute = bcd(minute);
		second = bcd(second);
	}

	if(!(status & 0x02) && (hour & 0x80)){
		hour = ((hour & 0x7F) + 12) % 24;
	}

	int32_t y = 2000 + year;
	int32_t unix_timecode = make_timecode(y, month, day, hour, minute, second);

	return unix_timecode;
}

int32_t syscall_timestamp(Registers& registers){
	return current();
}

void initialize(){
	register_system_call(syscall_timestamp, SC_timestamp);
}

};
