
#include "gif2html.h"

// s_identity[0] == 0, s_identity[188] = 188, etc
static uchar s_identity[256];


LZW::LZW(unsigned int givenSize, unsigned int outputBufferSize) {
	_bits = _bitsLeft = 0;

	for(int i = 0; i < 256; ++i)
		s_identity[i] = i;

	_outBegin = (uchar*) malloc(outputBufferSize+1);
	assert(_outBegin);
	_outp = _outBegin;
	_outEnd = _outBegin + outputBufferSize;

	_startCodeSize = givenSize;
	_reset();

	_lastCode = _clearCode; // code which produces 0 characters
	_lastOutput = _outBegin;

	// XXX does clear code have to come first? if so, bother enforcing?
}


LZW::~LZW() {
	free(_outBegin);
}


void LZW::_reset() {
	assert(_startCodeSize >= 2);   // "ESTABLISH CODE SIZE" in GIF89a spec, Appendix F
	assert(_startCodeSize <= 11);

	_dictNextCode = 0;

	memset(_dict, 0, sizeof _dict);

	while(_dictNextCode < (1u<<_startCodeSize)) {
		_dict[_dictNextCode].len = 1;
		_dict[_dictNextCode].p = s_identity + (_dictNextCode++);
	}

	_clearCode = (_dictNextCode++);
	_endCode = (_dictNextCode++);
	_currentSize = _startCodeSize+1;
}


bool LZW::feed(uchar c) {
	unsigned int i;

	printf("    LZW fed byte 0x%02x\n", c);
	_bits |= ((unsigned int)c) << _bitsLeft;
	_bitsLeft += 8;

	while(_bitsLeft >= _currentSize) {
		// XXX we don't special case first code - will break if it's not a reset!
		unsigned int code = _bits & ((1u<<_currentSize)-1);
		_bits >>= _currentSize;
		_bitsLeft -= _currentSize;

		printf("      bin %s code 0x%3x ", itob(code,_currentSize,12).c_str(), code);

		if(_dict[code].len || code == _dictNextCode) {
			// add to the dictionary, unless this is the first code, or the
			// dictionary is already as large as is permitted
			if(_dict[_lastCode].len && _dictNextCode < 4096) {
				_dict[_dictNextCode].len = _dict[_lastCode].len+1;
				_dict[_dictNextCode].p   = _lastOutput;
				_dictNextCode++;

				// bearing in mind that _dictNextCode can validly appear (although
				// it is not yet defined), figure out how many bits the next code
				// will take up
				if(_dictNextCode >= (1u<<_currentSize) && _dictNextCode < 4096) {
					++_currentSize;
					printf("dictionary is now %0d entries, _currentSize = %0d\n",
						_dictNextCode, _currentSize);
				}
			}

			_lastOutput = _outp;

			printf("  BYTES:  ");
			for(i = 0; i < _dict[code].len; ++i) {
				unsigned char pixel = _dict[code].p[i];

				*_outp++ = pixel;
				printf("%0x ", (int) pixel);
				WRITER->pixel(pixel);
			}
			printf("\n");

			*_outp = _dict[code].p[0];   // needed if the next input code == _dictNextCode
		} else if(code == _clearCode) {
			printf("    reset dictionary\n");
			_reset(); // XXX reset _oldCode?
		} else if(code == _endCode) {
			printf("    end of stream; %0d spare bits\n", _bitsLeft);
			return false;
		} else {
			printf("    unknown code!\n");
			die("unknown code in compressed data");
		}

		_lastCode = code;
	}

	return true;
}

	


	
