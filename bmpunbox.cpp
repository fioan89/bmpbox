#include <string>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include "bmpunbox.h"

BMPUnbox::BMPUnbox(std::string fileName)
{
    if (fileName.find_last_of(".bmp") == std::string::npos)
    {
        fprintf(stderr, "[ debug ] Error: file %s does not look like a valid BMP image. File extension is not .bmp!\n", fileName.c_str());
        return;
    }

    in = fopen(fileName.c_str(), "r");
    if (in == NULL)
    {
        fprintf(stderr, "[ debug ] Error: file %s could not be opened!\n", fileName.c_str());
        return;
    }

    // seek over header and info
    fseek(in, sizeof(bmpheader_t), SEEK_SET);
    fseek(in, sizeof(bmpinfo_t), SEEK_CUR);
    // compute palette size
    uint32_t nrOfColors = pow(2, BITDEPTH);
    fseek(in, sizeof(rgba_t) * nrOfColors, SEEK_CUR);
}

size_t BMPUnbox::readBytes(uint8_t *buffer, uint32_t length)
{
    return fread(buffer, sizeof(uint8_t), length, in);
}


BMPUnbox::~BMPUnbox()
{
    fclose(in);
}
