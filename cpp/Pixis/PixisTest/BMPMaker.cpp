#include "BMPMaker.h"


BMPMaker::BMPMaker(const char* filename, int xRez, int yRez, unsigned short *BGR, int length)
{
	short padding = 0x0000;
	short BM = 0x4d42;
	int w_in_pix = xRez;
	int h_in_pix = yRez;

	HEADER.size_of_file = sizeof(HEADER) + 24*xRez*yRez;
	HEADER.reserve = 0000;
	HEADER.offset_of_pixle_data = 54;
	HEADER.size_of_header = 40;
	HEADER.width = xRez;
	HEADER.hight = yRez;
	HEADER.num_of_colour_plane = 1;
	HEADER.num_of_bit_per_pix = 24;
	HEADER.compression = 0;
	HEADER.size_of_pix_data = sizeof(unsigned short)*length + sizeof(padding) * h_in_pix;
	HEADER.h_resolution = 0;
	HEADER.v_resolution = 0;
	HEADER.num_of_colour_in_palette = 256;
	HEADER.important_colours = 0;


	// write BMP Header ////////////////////////////////////////////////////////////////
	ofstream file;
	file.open (filename, ios::out | ios::trunc | ios::binary);
	file.write ((char*)(&BM), 2);
	file.write ((char*)(&HEADER), sizeof(HEADER));
	////////////////////////////////////////////////////////////////////////////////////
	// write BMP data //////////////////////////////////////////////////////////////////
	//file.write ((char*)(&BGR[0]),1);
	int position = xRez*yRez;
	unsigned short temp;
	char* output;
	while (position != 0)
	{
		for (int n = 0; n != w_in_pix; n++)
		{
			temp = (BGR[position - w_in_pix + n] & 0xff00) >> 8;
			//temp = BGR[position - w_in_pix + n];
			file.write ((char*)(&temp), 1);
			file.write ((char*)(&temp), 1);
			file.write ((char*)(&temp), 1);
		}
		//file.write ((char*)(&padding), 2);
		position = position - w_in_pix;
	}
}
