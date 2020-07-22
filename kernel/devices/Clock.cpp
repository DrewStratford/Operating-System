#include <devices/Clock.h>
#include <devices/IO.h>
#include <devices/Serial.h>

constexpr uint8_t cmos_port = 0x70;
constexpr uint8_t cmos_data = 0x71;

constexpr uint8_t cmos_year = 0x09;
constexpr uint8_t cmos_month = 0x08;
constexpr uint8_t cmos_day = 0x07;
constexpr uint8_t cmos_hour = 0x04;
constexpr uint8_t cmos_minute = 0x02;
constexpr uint8_t cmos_second = 0x00;

constexpr uint8_t cmos_status = 0x0B;

constexpr int32_t unix_epoch = 1970;
constexpr uint32_t seconds_per_day = 86400;

uint32_t years_since_epoch(uint32_t year){
	return year - unix_epoch;
}

int32_t seconds_till_year(int32_t year){
	int32_t normal = 365 * seconds_per_day;
	int32_t leap = 366 * seconds_per_day;
	
	int32_t delta = years_since_epoch(year);
	return delta * normal - (delta/4) * normal + (delta/4) * leap;
}

int32_t seconds_till_month(int32_t year, int32_t month){
	int32_t days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int32_t seconds = 0;

	if(year % 4 == 0)
		days[2]++;

	for(int i = 0; i < month; i++)
		seconds+= days[i] * seconds_per_day;

	return seconds;
}

static uint8_t read_cmos(uint8_t reg){
	IO::out8(cmos_port, reg);
	return IO::in8(cmos_data);
}

static uint8_t bcd(uint8_t byte){
	uint8_t high = byte >> 4;
	uint8_t low = byte & 0xF;
	return (high * 10) + low;
}

int32_t to_years(int32_t time){
	int32_t normal = 365 * seconds_per_day;
	int32_t leap = 366 * seconds_per_day;
	int32_t year = unix_epoch;

	com1() << time << " --> ";
	// Calculate year.
	while(time >= normal || ((year % 4 == 0) && time >= leap)){
		if(year % 4 == 0)
			time-= leap;
		else
			time-= normal;

		year++;
	}
	com1() << "Y:"<< year;

	// Calculate month.
	int32_t months[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int32_t month = 0;

	if(year % 4 == 0)
		months[2]++;

	for(;month < 12 && time >= months[month] * seconds_per_day; month++)
		time-= months[month] * seconds_per_day;

	// Calculate day.
	int32_t day = time / seconds_per_day;
	time-= day * seconds_per_day;
	day++;

	// Calculate hour
	int32_t hour = time / (60*60);
	time-= hour * 60 * 60;

	// Calculate minute
	int32_t minute = time / 60;
	time-= minute * 60;

	// Calculate second
	int32_t second = time;

	return year;
}

int32_t make_timecode(int32_t y, int32_t m, int32_t d, int32_t h, int32_t min, int32_t s){
	return seconds_till_year(y) + seconds_till_month(y, m) +
		(d-1)*seconds_per_day + 
		h*60*60 + min*60 + s;
}

int32_t current_timestamp(){
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
