

#include "gif2html.h"

HTMLWriter* WRITER;

void processGif(Stream& s) {
	//
	// GIF89a SPEC SECTION 17 - HEADER
	//
	if(s.string(6) != "GIF89a") 
		die("doesn't have GIF89a header");

	//
	// GIF89a SPEC SECTION 18 - LOGICAL SCREEN DESCRIPTOR
	//
	unsigned int width  = (int) s.byte() + 256*s.byte();
	unsigned int height = (int) s.byte() + 256*s.byte();
	WRITER->setImageDimensions(width, height);
	printf("WIDTH: %0d\nHEIGHT: %0d\n", width, height);

	int gctSize;
	printf("stated size of global color table: %0d\n", (gctSize = (int) s.bits(3)));
	printf("sort flag: %0d\n", (int) s.bits(1));
	printf("color resolution: %0d\n", (int) s.bits(3));
	if(s.bits(1)) {
		printf("global color table flag is set\n");
	} else {
		printf("no global color table present\n");
		gctSize = -1;
	}

	printf("background color index: %0d\n", (int) s.byte());

	int ratio = (int) s.byte();
	if(ratio) 
		printf("aspect ratio = (%0d + 15)/16 = %.4f\n", ratio, (15.0f + (float)ratio) / 64.0f);
	else
		printf("aspect ratio = 0 = none\n");

	//
	// GIF89a SPEC SECTION 19 - GLOBAL COLOR TABLE
	//
	if(gctSize >= 0) {
		int n = (1 << (gctSize+1)) * 3;
		vector<uchar> palette = s.bytes(n);
		WRITER->setPalette(palette);
		printf("read %0d byte gct\n", n);
	}

	//
	// Many different blocks can follow
	//
	while(!s.eof()) {
		int blockType = (int) s.byte();
		switch(blockType) {
		case 0x3B:
			//
			// GIF89a SPEC SECTION 20 - IMAGE DESCRIPTOR
			//
			printf("GIF trailer 0x3B\n\n");
			assert(s.eof());
			break;

		case 0x2C: {
			//
			// GIF89a SPEC SECTION 20 - IMAGE DESCRIPTOR
			//
			printf("image descriptor separator byte: 0x2C\n");
	
			printf("image left: %0d\n", (int)s.byte() + 256*s.byte());
			printf("image top: %0d\n", (int)s.byte() + 256*s.byte());
			printf("image width: %0d\n", (int)s.byte() + 256*s.byte());
			printf("image height: %0d\n", (int)s.byte() + 256*s.byte());
	
			int lctSize;
			printf("stated local color table size: %0d\n", (lctSize = s.bits(3)));
			printf("reserved data area: %0d\n", (int) s.bits(2));
			printf("sort flag: %0d\n", (int) s.bits(1));
			printf("interlace flag: %0d\n", (int) s.bits(1));
			if(s.bits(1)) {
				printf("local color table flag is set\n");
			} else {
				printf("no local color table present\n");
				lctSize = -1;
			}
	
			//
			// GIF89a SPEC SECTION 21 - LOCAL COLOR TABLE
			//
			if(lctSize >= 0) {
				int n = (1 << (lctSize+1)) * 3;
				printf("skipping %0d byte lct\n", n);
				s.bytes(n);
			}
	
			//
			// GIF89a SPEC SECTION 21 - TABLE BASED IMAGE DATA
			//
			printf("LZW minimum code size: %0d\n", (int) s.peek());
			LZW lzw((unsigned int) s.byte(), 1024u + width*height);
			unsigned char dataSize;
			bool expecting = true;
			do {
				dataSize = s.byte();
				printf("  image data subblock, %0d bytes\n", (int) dataSize);
				int i;
				for(i = 0; expecting && i < (int) dataSize; ++i) {
					 expecting = lzw.feed(s.byte());
				}
				if(i != (int) dataSize) {
					die("Error: end-of-image code in middle of data subblock");
				}
			} while(dataSize);

			if(expecting) {
				die("Error: empty data sub-block before end-of-image code");
			}

			break;
		}
	
		case 0x21: {
			uchar label = s.byte();
			vector<uchar> v;
			int dataBlocks = -1;
			unsigned char blockSize;
	
			do {
				blockSize = s.byte();
				s.bytes(v, (int) blockSize);
				++dataBlocks;
			} while(blockSize);
	
			printf("extension (0x21) block, type 0x%02x, contains %0d bytes in %0d blocks\n",
					(int) label, (int) v.size(), dataBlocks);
			
			switch(label) {
			case 0xF9:
				assert(v.size() == 4);
				if(v[0] & 1) {
					printf("  transparent color: 0x%02x\n", (int) v[3]);
					WRITER->setTransparent(v[3]);
				} else {
					printf("  no transparency\n");
				}
				break;
			}
	
			break;
		}
	
		default:
			printf("unrecognised block type 0x%02x\n!", blockType);
			die("");
			break;
	
		} // switch
	} // while(!s.eof())
	printf("end of file reached\n\n");
}

int main(int argc, char** argv)
{
	WRITER = new HTMLWriter();

	Stream s = Stream::fromFile("-");
	processGif(s);
	
	FILE* fp = fopen("out.html", "w");
	assert(fp);
	WRITER->save(fp);
	fclose(fp);

	delete WRITER;

	return 0;
}

