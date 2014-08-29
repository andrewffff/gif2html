
#include "gif2html.h"

HTMLWriter::HTMLWriter() {
	_transparent = -1;
}


void HTMLWriter::save(FILE* fp) {
	unsigned int i;

	fprintf(fp, "<!DOCTYPE html>\n");
	fprintf(fp, "<html>\n");
	fprintf(fp, "<!--\n");
	fprintf(fp, "      This file was produced by gif2html. It contains data derived from\n");
	fprintf(fp, "      an input GIF provided by the user, and may also contain parts of\n");
	fprintf(fp, "      gif2html. The input GIF may be copyrighted, and the gif2html code\n");
	fprintf(fp, "      will be separately copyrighted, under the terms of the GNU GPL v2.\n");
	fprintf(fp, "\n");
	fprintf(fp, "      This could mean that the combined file is GPLed, or copyrighted by\n");
	fprintf(fp, "      the input GIF's copyright holder, or some impossible combination of\n");
	fprintf(fp, "      the two. It may not be possible to legally redistribute this file!\n");
	fprintf(fp, "\n");
	fprintf(fp, "      Comments throughout the file attempt to demarcate different blocks\n");
	fprintf(fp, "      of material, and identify their source.\n");
	fprintf(fp, "-->\n");
	fprintf(fp, "<head>\n");
	fprintf(fp, "\n");
//	fprintf(fp, "<link rel='stylesheet' type='text/css' src='g2h_rt_style.css'>\n");
	fprintf(fp, "<style>\n");
	fprintf(fp, "    /* BEGIN: Embedded copy of g2h_rt_style.css, part of gif2html */\n");
	fprintf(fp, "DIV.pixels { margin: 0px; border: 0px; padding: 0px; line-height: 0px; }\n");
	fprintf(fp, ".pix { display: inline-block; width: 4px; height: 4px; border: 1px solid black; margin: 0px; padding: 0px;}\n");
	fprintf(fp, ".c-t { border: 1px solid #99D; background-image: url('g2h_rt_trans.gif'); }\n");
	fprintf(fp, "    /* END: Embedded copy of g2h_rt_style.css, part of gif2html */\n");
	fprintf(fp, "</style>\n");
	fprintf(fp, "<style>\n");
	fprintf(fp, "    /* BEGIN: Palette information from the original GIF file. */\n");
	for(i = 0; i < _palette.size()/3; ++i) {
		fprintf(fp, ".c-%0d { background: #%02x%02x%02x; }\n",
			i, (int) _palette[3*i], (int) _palette[3*i + 1], (int) _palette[3*i + 2]);
	}
	fprintf(fp, "    /* END: Palette information from the original GIF file. */\n");
	fprintf(fp, "</style>\n");
	fprintf(fp, "</head>\n");
	fprintf(fp, "<body>\n");
	fprintf(fp, "    <!-- BEGIN: Pixel data (the actual graphic) from the original GIF file. -->\n");
	fprintf(fp, "<div class='pixels'>\n");
	for(i = 0; i < _pixels.size(); ++i) {
		int c = (int) _pixels[i];

		if(c == _transparent)
			fprintf(fp, "<div id='p-%0d' class='pix c-t'></div>", i);
		else
			fprintf(fp, "<div id='p-%0d' class='pix c-%0d'></div>", i, c);

		if(0 == ((i+1) % _width)) {
			fprintf(fp, "<br>\n");
		}
	}
	fprintf(fp, "</div>\n");
	fprintf(fp, "    <!-- END: Pixel data (the actual graphic) from the original GIF file. -->\n");
	fprintf(fp, "</body>");
	fprintf(fp, "</html>");
}


void HTMLWriter::setPalette(const vector<uchar>& bytes) {
	assert(bytes.size() > 0);
	assert(bytes.size() < 768);
	assert(0 == (bytes.size() % 3));
	_palette = bytes;
}


void HTMLWriter::setTransparent(uchar c) {
	_transparent = (int) c;
}

void HTMLWriter::setImageDimensions(unsigned int width, unsigned int height) {
	_width = width;
	_height = height;
}


void HTMLWriter::byteRead(uchar c) {
	return;
}


void HTMLWriter::bytePartiallyUsed() {
	return;
}


void HTMLWriter::pixel(uchar c) {
	_pixels.push_back(c);
}

