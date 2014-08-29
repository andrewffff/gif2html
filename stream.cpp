
#include "gif2html.h"

Stream::Stream(const vector<uchar>& in)
: _raw(in), _p(_raw.begin()), _bits(0), _bitsLeft(0)
{
	return;
}

	
vector<uchar> Stream::bytes(int n) {
	vector<uchar> ret;
	bytes(ret, n);
	return ret;
}


void Stream::bytes(vector<uchar>& dest, int n) {
	assert(!_bitsLeft);
	assert(_p + n <= _raw.end());

	while(n--) {
		dest.push_back(*_p++);
	}
}


uchar Stream::byte() {
	assert(!_bitsLeft);
	assert(_p != _raw.end());
	return *_p++;
}


uchar Stream::peek() const {
	assert(!_bitsLeft);
	assert(_p != _raw.end());
	return *_p;
}


std::string Stream::string(int n) {
	assert(!_bitsLeft);
	std::vector<uchar>::const_iterator x = _p;

	_p += n;
	assert(_p <= _raw.end());

	return std::string((const char*) &*x, n);
}


unsigned int Stream::bits(int n) {
	while(_bitsLeft < n) {
		assert(_p != _raw.end());
		_bits |= ((unsigned int)(*_p++) << _bitsLeft);
		_bitsLeft += 8;
	}

	unsigned int ret = _bits & ((1<<n)-1);
	_bits >>= n;
	_bitsLeft -= n;
	return ret;
}


bool Stream::eof() const {
	return !_bitsLeft && (_p == _raw.end());
}


Stream Stream::fromFile(const char* filename) {
	FILE* fp = (filename[0] == '-' && filename[1] == '\0') ? stdin : fopen(filename, "rb");
	if(!fp) {
		fprintf(stderr, "Couldn't open %s\n", filename);
		exit(1);
	}

	vector<uchar> v;
	static bool warned = 0;

	while(!feof(fp)) {
		char buf[4096];
		size_t n = fread(buf, 1, sizeof(buf), fp);
		if(ferror(fp)) {
			fprintf(stderr, "I/O error on read, dying!\n");
			exit(1);
		}
		for(size_t i = 0; i < n; ++i) {
			v.push_back(buf[i]);
		}

		if(v.size() > 512*1024 && !warned) {
			warned = true;
			fprintf(stderr, "WARNING: read over 512k, are we in the right place?\n");
		}
	}

	if(fp != stdin) {
		fclose(fp);
	}

	return Stream(v);
}

