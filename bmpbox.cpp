#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include "bmpbox.h"

BMPBox::BMPBox(std::string fileName)
{
    bit_depth = BITDEPTH;
    nr_of_colors = pow(2, bit_depth);
    aditionalBytes = 0;

    in = fopen(fileName.c_str(), "r");
    if (in == NULL)
    {
        fprintf(stderr, "[ debug ] Error: file %s could not be opened!\n", fileName.c_str());
        return;
    }
    size_t sz = getFileSzInBytes(in);
    uint32_t* primes;
    primes = findTwoDivisors(sz);
    height = primes[0];
    width = primes[1];
    free(primes);

    fprintf(stderr, "[ debug ] Info: size:%d height:%d width:%d\n",sz, height, width);
    fprintf(stderr, "[ debug ] Info: aditional bytes to add:%d\n", aditionalBytes);

    // create bmp picture
    header.magic[0] = 'B';
    header.magic[1] = 'M';
    header.creator1 = 0;
    header.creator2 = 0;

    info.header_sz = 40;
    info.width = width;
    info.height = height;
    info.nplanes = 1;
    info.depth = bit_depth;
    info.hres = DEFAULT_DPI_X;
    info.vres = DEFAULT_DPI_Y;
    info.compress_type = 0;

    palette = allocateColorTable();
    genColorTable(palette);

    /* Calculate the field value of header and DIB */
    double bytes_per_pixel = (bit_depth * 1.0) / 8.0;
    uint32_t bytes_per_line = (int)ceil(bytes_per_pixel * width);
    if (bytes_per_line % 4 != 0)
    {
        bytes_per_line += 4 - bytes_per_line % 4;
    }
    info.bmp_bytesz = bytes_per_line * height;
    uint32_t palette_size = 0;
    palette_size = pow(2, bit_depth) * 4;
    header.offset = 14 + info.header_sz + palette_size;
    header.filesz = header.offset + info.bmp_bytesz;
}

uint32_t BMPBox::getAditionalBytes()
{
    return aditionalBytes;
}

uint32_t* BMPBox::findTwoDivisors(uint32_t number)
{
    uint32_t* primes = (uint32_t*) malloc (sizeof(uint32_t) * 2);
    primes[0] = 1;
    primes[1] = number;
    primes[0] = round(floor(sqrt(number)));
    primes[1] = primes[0];

    while (primes[0] * primes[1] < number)
    {
        primes[0] +=1;
    }
    aditionalBytes = (primes[0] * primes[1]) - number;

    return primes;
}

size_t BMPBox::getFileSzInBytes(FILE *fp)
{
    if (fp == NULL)
    {
        return 0;
    }

    size_t sz = 0;
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    rewind(fp);
    return sz;
}

uint8_t BMPBox::getBitDepth()
{
    return bit_depth;
}

uint32_t BMPBox::getNumberOfColors()
{
    return nr_of_colors;
}

rgba_t* BMPBox::allocateColorTable()
{
    info.ncolors = getNumberOfColors();
    rgba_t * buffer = (rgba_t*) malloc(sizeof(rgba_t)* getNumberOfColors());
    return buffer;
}

void BMPBox::genColorTable(rgba_t *colors)
{
    unsigned int i;
    uint8_t step_size;

    if (!colors)
    {
        fprintf(stderr, "[ debug ] Error: Could not generate color table because buffer for colors is null!\n");
        return;
    }

    if (getBitDepth() != 1)
        step_size = 255 / (nr_of_colors - 1);
    else
        step_size = 255;

    for (i = 0; i < getNumberOfColors(); ++i) {
        uint8_t value = i * step_size;
        rgba_t color = {value, value, value, 0};
        colors[i] = color;
    }
}

bool BMPBox::isBigEndian()
{
    uint16_t value = 0x0001;
    return (*(uint8_t *)&value) != 0x01;
}

void BMPBox::swapHeaderEndianess(bmpheader_t *header)
{
    header->filesz = UINT32_SWAP_ENDIANESS(header->filesz);
    header->creator1 = UINT16_SWAP_ENDIANESS(header->creator1);
    header->creator2 = UINT16_SWAP_ENDIANESS(header->creator2);
    header->offset = UINT32_SWAP_ENDIANESS(header->offset);
}

void BMPBox::swapInfoEndianess(bmpinfo_t *info)
{
    info->header_sz = UINT32_SWAP_ENDIANESS(info->header_sz);
    info->width = UINT32_SWAP_ENDIANESS(info->width);
    info->height = UINT32_SWAP_ENDIANESS(info->height);
    info->nplanes = UINT16_SWAP_ENDIANESS(info->nplanes);
    info->depth = UINT16_SWAP_ENDIANESS(info->depth);
    info->compress_type = UINT32_SWAP_ENDIANESS(info->compress_type);
    info->bmp_bytesz = UINT32_SWAP_ENDIANESS(info->bmp_bytesz);
    info->hres = UINT32_SWAP_ENDIANESS(info->hres);
    info->vres = UINT32_SWAP_ENDIANESS(info->vres);
    info->ncolors = UINT32_SWAP_ENDIANESS(info->ncolors);
    info->nimpcolors = UINT32_SWAP_ENDIANESS(info->nimpcolors);
}

void BMPBox::writeHeader(bmpheader_t *header)
{
    if (isBigEndian())
    {
        fprintf(stderr, "[ debug ] Info: your architecture is big endian. Going to switch header to little endian!\n");
        swapHeaderEndianess(header);
    }

    fwrite(header->magic, sizeof(uint8_t), 2, out);
    fwrite(&(header->filesz), sizeof(uint32_t), 1, out);
    fwrite(&(header->creator1), sizeof(uint16_t), 1, out);
    fwrite(&(header->creator2), sizeof(uint16_t), 1, out);
    fwrite(&(header->offset), sizeof(uint32_t), 1, out);
}

void BMPBox::writeInfo(bmpinfo_t *info)
{
    if (isBigEndian())
    {
        fprintf(stderr, "[ debug ] Info: your architecture is big endian. Going to switch info to little endian!\n");
        swapInfoEndianess(info);
    }

    fwrite(&(info->header_sz), sizeof(uint32_t), 1, out);
    fwrite(&(info->width), sizeof(uint32_t), 1, out);
    fwrite(&(info->height), sizeof(uint32_t), 1, out);
    fwrite(&(info->nplanes), sizeof(uint16_t), 1, out);
    fwrite(&(info->depth), sizeof(uint16_t), 1, out);
    fwrite(&(info->compress_type), sizeof(uint32_t), 1, out);
    fwrite(&(info->bmp_bytesz), sizeof(uint32_t), 1, out);
    fwrite(&(info->hres), sizeof(uint32_t), 1, out);
    fwrite(&(info->vres), sizeof(uint32_t), 1, out);
    fwrite(&(info->ncolors), sizeof(uint32_t), 1, out);
    fwrite(&(info->nimpcolors), sizeof(uint32_t), 1, out);
}

void BMPBox::writePalette(rgba_t *palette)
{

    unsigned int i;
    for (i = 0; i < nr_of_colors; ++i)
    {
        fwrite(&palette[i], sizeof(rgba_t), 1, out);
    }
}

void BMPBox::fillBufferFromRow(uint8_t *buffer, size_t length)
{
    if (width > length)
    {
        fprintf(stderr, "[ debug ] Error: Buffer length with value %d is less than width %d!\n", length, width);
        return;
    }

    // read data from file;
    fread(buffer, sizeof(uint8_t), length, in);

}

bool BMPBox::writeToFile(std::string fileName)
{

    int row;
    unsigned char *buf;

    /* Create the file */
    if ((out = fopen(fileName.c_str(), "wb")) == NULL)
    {
        fprintf(stderr, "[ debug ] Error: Could not write to file %s!\n", fileName.c_str());
        return false;
    }

    /* Write the file */
    writeHeader(&header);
    writeInfo(&info);
    writePalette(palette);

    double bytes_per_pixel;
    int bytes_per_line;

    bytes_per_pixel = (bit_depth * 1.0) / 8.0;
    bytes_per_line = (int)ceil(bytes_per_pixel * width);
    if (bytes_per_line % 4 != 0)
        bytes_per_line += 4 - bytes_per_line % 4;

    buf = (unsigned char *)malloc(bytes_per_line);

    for (row = height - 1; row >= 0; --row)
    {
        memset(buf, 0, bytes_per_line);
        fillBufferFromRow(buf, bytes_per_line);
        fwrite(buf, sizeof(uint8_t), bytes_per_line, out);
    }

    free(buf);
    fclose(out);
    return true;
}

BMPBox::~BMPBox()
{
    fclose(in);
    // free palette
    if (palette)
    {
        free(palette);
    }
}
