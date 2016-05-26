#pragma once

struct searchresult_t {
public:
	unsigned char success;

	// we need some space or the search will fuck everything up
	char padding[512];
};
