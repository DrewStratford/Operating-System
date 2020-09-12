#pragma once

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>

template<size_t SIZE>
class Bitmap{
public:
	Bitmap(){
		clear_all();
	}

	void set(size_t index){
		if (index >= SIZE || is_set(index))
	  		return;

		size_t group = index / 32;
		size_t remainder = 31 - (index % 32);

	  	map[group] |= (1 << remainder);
		m_count++;
	}

	bool is_set(size_t index){
		if (index >= SIZE)
	  		return false;

		size_t group = index / 32;
		size_t remainder = 31 - (index % 32);

	  	return map[group] & (1 << remainder);
	}

	void clear(size_t index){
		if (index >= SIZE || !is_set(index))
	  		return;

		size_t group = index / 32;
		size_t remainder = 31 - (index % 32);

	  	map[group] &= ~(1 << remainder);
		m_count--;
	}

	void clear_all(){
		for(size_t i = 0; i < SIZE / 32; i++){
			map[i] = 0;
		}
		m_count = 0;
	}

	size_t find_free(){
		for(size_t i = 0; i < SIZE / 32; i++){
			if(map[i] == 0xFFFFFFFF)
				continue;
			if(map[i] == 0)
				return (i * 32);

			size_t out = 0;
			asm("bsr %[i], %[o]"
				: [o] "=irm"(out)
				: [i] "irm"(~map[i])
				);
			return (i * 32) + (31 - out);
		}
		return -1;
	}

	bool is_empty(){
		return m_count == SIZE;
	}

private:
	uint32_t map[SIZE / 32];
	size_t m_count { 0 };
};
