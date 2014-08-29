
#include "gif2html.h"

void die(const char* s) {
	fprintf(stderr, "%s\n", s);
	exit(2);
}


std::string itob(unsigned int n, unsigned int leftPadded, unsigned int rightPadded) {
	if(!n && !leftPadded) {
		leftPadded = 1; // avoid "" being the correct result
	}

	unsigned int mask = 1<<31;
	assert(mask > 0);
	assert(mask > n);
	assert(leftPadded < 30);

	while(!(n&mask) && (mask > (1u<<(leftPadded-1))))
		mask >>= 1;

	string ret;
	while(mask > 0) {
		ret.push_back((n&mask) ? '1' : '0');
		mask >>= 1;
	}

	while(ret.size() < rightPadded) {
		ret.push_back(' ');
	}

	return ret;
}


