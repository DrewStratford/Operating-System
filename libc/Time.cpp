#include <Time.h>

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

Time from_timecode(int32_t time){
	Time out;
	int32_t normal = 365 * seconds_per_day;
	int32_t leap = 366 * seconds_per_day;
	int32_t year = unix_epoch;

	// Calculate year.
	while(time >= normal || ((year % 4 == 0) && time >= leap)){
		if(year % 4 == 0)
			time-= leap;
		else
			time-= normal;

		year++;
	}

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

	out.year = year;
	out.month = month;
	out.day = day;
	out.hour = hour;
	out.minute = minute;
	out.second = second;
	return out;
}

int32_t make_timecode(int32_t y, int32_t m, int32_t d, int32_t h, int32_t min, int32_t s){
	return seconds_till_year(y) + seconds_till_month(y, m) +
		(d-1)*seconds_per_day + 
		h*60*60 + min*60 + s;
}
