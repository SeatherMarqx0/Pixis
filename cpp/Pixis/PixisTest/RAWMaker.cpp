#include "RAWMaker.h"

RAWMaker::RAWMaker()
{
    sequenceStarted = false;
}

RAWMaker::~RAWMaker(void)
{
    endImageSequence();
}


void RAWMaker::createSingleImage(const char* filename, int xRez, int yRez, unsigned short* buffer, int pixelsize_bytes)
{
    FILE *data = fopen( filename, "wb" );
    /* write the frame to the file */
    fwrite( buffer, pixelsize_bytes, (xRez*yRez), data );
    fclose( data );
}

void RAWMaker::startImageSequence(const char* filename, int xRez, int yRez, int pixelsize_bytes)
{
    sequenceStarted = true;
    seqFile = fopen(filename, "wb");
    xRezSeq = xRez;
    yRezSeq = yRez;
    pixelsize_bytesSeq = pixelsize_bytes;
}
void RAWMaker::insertImageIntoSequence(unsigned short* buffer)
{
    fwrite( buffer, pixelsize_bytesSeq, (xRezSeq*yRezSeq), seqFile );
}

void RAWMaker::endImageSequence()
{
    sequenceStarted = false;
    fclose( seqFile );
}