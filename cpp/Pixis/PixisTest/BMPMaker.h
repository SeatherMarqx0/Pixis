#pragma once

#include <fstream>
#include <iostream>

using namespace std;

class BMPMaker
{


struct bmp_header
{
	long size_of_file;
	long reserve;
	long offset_of_pixle_data;
	long size_of_header;
	long width;
	long hight;
	short num_of_colour_plane;
	short num_of_bit_per_pix;
	long compression;
	long size_of_pix_data;
	long h_resolution;
	long v_resolution;
	long num_of_colour_in_palette;
	long important_colours;
}
HEADER;

public:
	BMPMaker(const char* filename, int xRez, int yRez, unsigned short *BGR, int length);
};
