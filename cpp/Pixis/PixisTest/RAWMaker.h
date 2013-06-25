#pragma once

#include <fstream>
#include <iostream>

class RAWMaker
{
private:
    FILE *seqFile;
    bool sequenceStarted;
    int xRezSeq;
    int yRezSeq;
    int pixelsize_bytesSeq;

public:
    RAWMaker(void);
    ~RAWMaker(void);
    static void createSingleImage(const char* filename, int xRez, int yRez, unsigned short* buffer, int pixelsize_bytes);
    void startImageSequence(const char* filename, int xRez, int yRez, int pixelsize_bytes);
    void insertImageIntoSequence(unsigned short* buffer);
    void endImageSequence();
};
