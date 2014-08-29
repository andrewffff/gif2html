
#include <string>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;

typedef unsigned char uchar;

class HTMLWriter;
extern HTMLWriter* WRITER;

//
// lzw.cpp
//
class LZW {
private:
	// The output buffer. Note that the dictionary points into here
	uchar* _outBegin;

	// Current offset within output buffer
	uchar* _outp;

	// One past the end of the output buffer
	uchar* _outEnd;

	// GIF's LZW scheme maxes out at code 4095
	struct _DictionaryItem { const uchar *p; unsigned int len; } _dict[4096];

	// info about the dictionary:
	unsigned int _startCodeSize; // initial code size (in bits). note: immediately bumped up by 2 codes..
	unsigned int _currentSize;   // number of bits to take from the stream
	unsigned int _dictNextCode;  // number of codes currently in the dictionary
	unsigned int _clearCode, _endCode;  // magic codes

	// the last code that was received
	unsigned int _lastCode;

	// the start of the output that was produced by _lastCode
	uchar* _lastOutput;

	// left-over bits that were packed into the last byte we received
	unsigned int _bits;
	unsigned int _bitsLeft;

	// resets the dictionary to the start state, defined by _givenSize
	void _reset();
public:
	/**
	 * Call this with the LZW code size that precedes the first data subblock.
	 * (GIF89a spec Appendix F)
	 *
	 * You must also specify an upper limit for decompression output.
	 */
	LZW(unsigned int givenSize, unsigned int outputBufferSize);

	/** Destructor */
	~LZW();

	/**
	 * Feed a data byte into the decoder.
	 *
	 * Returns true if more bytes should be expected, or false
	 * if the end-of-image code is encountered.
	 */
	bool feed(uchar c);
};


//
// stream.cpp
//
class Stream {
private:
	const vector<uchar> _raw;
	vector<uchar>::const_iterator _p;

	unsigned int _bits;
	int _bitsLeft;

public:
	/** Constructor */
	Stream(const vector<uchar>& in);

	/** Returns the next n bytes in the stream */
	vector<uchar> bytes(int n);

	/** Appends the next n bytes in the stream to the given vector */
	void bytes(vector<uchar>& dest, int n);

	/** Returns the next available byte */
	uchar byte();

	/** Peek at the next byte without advancing our position */
	uchar peek() const;

	/** Returns a string from the next n characters in the stream */
	std::string string(int n);

	/** Returns the next available n bits */
	unsigned int bits(int n);

	/** Have we reached the end? */
	bool eof() const;

	static Stream fromFile(const char* filename);
};



//
// util.cpp
//

/**
 * Print the error message and abort the program.
 */
void die(const char* s);


/**
  * returns a string which represents the input stream in binary,
  * optionally padded out front with 0 to be at least leftPadded
  * characters long, and out back with ' ' to be at least rightPadded
  * characters long
  *
  * eg itob(125, 0, 8) is "1111101 "
  */
std::string itob(unsigned int n, unsigned int leftPadded = 0, unsigned int rightPadded = 0);


//
// htmlwriter.cpp
//
class HTMLWriter {
private:
	vector<uchar> _palette;
	vector<uchar> _pixels;
	int _transparent;
	unsigned int _width, _height;

public:
	HTMLWriter();

	/**
	 * Write out the document.
	 */
	void save(FILE* fp);

	/**
	 * Tell the HTML writer what the palette is. We keep a copy.
	 */
	void setPalette(const vector<uchar>& bytes);

	/**
	 * Tell the HTML writer a particular color means transparent.
	 */
	void setTransparent(unsigned char c);

	/**
	 * Tell the HTML writer the image dimensions.
	 */
	void setImageDimensions(unsigned int width, unsigned int height);

	/**
	 * Tell that HTML writer that a byte is being read.
	 */
	void byteRead(uchar c);

	/**
	 * Tell the HTML writer that some bits from the most recently
	 * read byte are being retained for later use.
	 */
	void bytePartiallyUsed();

	/**
	 * Tell the HTML writer that a pixel has been decoded.
	 */
	void pixel(uchar c);
};


