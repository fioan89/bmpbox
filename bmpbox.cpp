#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <QFileInfo>

#include "bmpbox.h"

BMPBox::BMPBox(std::string fileName)
{
    fprintf(stderr, "#######################################################################\n");
    fileHeaderUsed = false;
    bufferIndex = 0;
    fileHeader = (geninfo_t*) malloc(sizeof(geninfo_t));
    memset(fileHeader, 0, sizeof(geninfo_t));
    header = (bmpheader_t*) malloc(sizeof(bmpheader_t));
    memset(header, 0, sizeof(bmpheader_t));
    info = (bmpinfo_t*) malloc(sizeof(bmpinfo_t));
    memset(info, 0, sizeof(bmpinfo_t));

    bitDepth = BITDEPTH;
    nrOfColorsInPalette = pow(2, bitDepth);
    additionalBytes = 0;

    in = NULL;
    in = fopen(fileName.c_str(), "rw+");
    if (in == NULL)
    {
        fprintf(stderr, "[ debug ] Error: file %s could not be opened!\n", fileName.c_str());
        return;
    }
    size_t sz = getFileSzInBytes(in);

    fileHeader->magic[0] = 'G';
    fileHeader->magic[1] = 'F';
    fileHeader->magic[2] = 'E';
    fileHeader->major_version = MAJOR_VERSION;
    fileHeader->minor_version = MINOR_VERSION;

    memset(fileHeader->full_name, '\n', 1024);
    memset(fileHeader->file_name, '\n', 256);
    memset(fileHeader->parent_directory, '\n', 1024);
    memset(fileHeader->extension, '\n', 255);

    QFileInfo fi(fileName.c_str());
    const char* fullName = fi.filePath().toStdString().c_str();
    const char* fileNm = fi.completeBaseName().toStdString().c_str();
    const char* parentDirectory = fi.absolutePath().toStdString().c_str();
    const char* extension = fi.completeSuffix().toStdString().c_str();

    strncpy(fileHeader->full_name, fullName, 1024);
    strncpy(fileHeader->file_name, fileNm, 256);
    strncpy(fileHeader->parent_directory, parentDirectory, 1024);
    strncpy(fileHeader->extension, extension, 255);

    fileHeader->data_size = sz;
    fileHeader->is_chunk = 0;
    fileHeader->is_public = 0;

    bufferToHoldFileHeader = reinterpret_cast<uint8_t*> (fileHeader);

    uint32_t* primes;
    primes = findTwoDivisors(sz + sizeof(geninfo_t));
    height = primes[0];
    width = primes[1];
    free(primes);

    fprintf(stderr, "[ debug ] Info: size:%d height:%d width:%d\n",sz, height, width);
    fprintf(stderr, "[ debug ] Info: aditional bytes to add:%d\n", additionalBytes);

    // create bmp picture
    header->magic[0] = 'B';
    header->magic[1] = 'M';
    header->creator1 = 0;
    header->creator2 = 0;

    info->header_sz = 40;
    info->width = width;
    info->height = height;
    info->nplanes = 1;
    info->depth = bitDepth;
    info->hres = DEFAULT_DPI_X;
    info->vres = DEFAULT_DPI_Y;
    info->compress_type = 0;

    palette = NULL;
    palette = allocateColorTable();
    genColorTable(palette);

    /* Calculate the field value of header and DIB */
    double bytes_per_pixel = (bitDepth * 1.0) / 8.0;
    uint32_t bytes_per_line = (int)ceil(bytes_per_pixel * width);
    if (bytes_per_line % 4 != 0)
    {
        bytes_per_line += 4 - bytes_per_line % 4;
    }
    info->bmp_bytesz = bytes_per_line * height;

    header->offset = 14 + info->header_sz + nrOfColorsInPalette * 4;
    header->filesz = header->offset + info->bmp_bytesz;
}

uint32_t BMPBox::getAditionalBytes()
{
    return additionalBytes;
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
    additionalBytes = (primes[0] * primes[1]) - number;

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
    return bitDepth;
}

uint32_t BMPBox::getNumberOfColors()
{
    return nrOfColorsInPalette;
}

rgba_t* BMPBox::allocateColorTable()
{
    info->ncolors = getNumberOfColors();
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
        step_size = 255 / (nrOfColorsInPalette - 1);
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
    fflush(out);
    fwrite(&(header->magic[0]), sizeof(uint8_t), 1, out);
    fwrite(&(header->magic[1]), sizeof(uint8_t), 1, out);
    fwrite(&(header->filesz), sizeof(uint32_t), 1, out);
    fwrite(&(header->creator1), sizeof(uint16_t), 1, out);
    fwrite(&(header->creator2), sizeof(uint16_t), 1, out);
    fwrite(&(header->offset), sizeof(uint32_t), 1, out);
    fflush(out);
}

void BMPBox::writeInfo(bmpinfo_t *info)
{
    if (isBigEndian())
    {
        fprintf(stderr, "[ debug ] Info: your architecture is big endian. Going to switch info to little endian!\n");
        swapInfoEndianess(info);
    }
    fflush(out);
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
    fflush(out);
}

void BMPBox::writePalette(rgba_t *palette)
{

    unsigned int i;
    for (i = 0; i < nrOfColorsInPalette; ++i)
    {
        fwrite(&palette[i], sizeof(rgba_t), 1, out);
        fflush(out);
    }
}

void BMPBox::fillBufferFromRow(uint8_t *buffer, size_t length)
{
    if (width > length)
    {
        fprintf(stderr, "[ debug ] Error: Buffer length with value %d is less than width %d!\n", length, width);
        return;
    }

    if (!fileHeaderUsed)
    {
        if ((sizeof(geninfo_t) - bufferIndex) <= length)
        {

            unsigned int i = 0;
            int counter = -1;
            for (i = bufferIndex; i < sizeof(geninfo_t); i++)
            {
                counter++;
                buffer[counter] = bufferToHoldFileHeader[i];
            }

            for (i = ++counter; i <length; i++)
            {
                fread(&buffer[i], sizeof(uint8_t), 1, in);
            }
            fileHeaderUsed = true;
        }
        else
        {
            memcpy(buffer, bufferToHoldFileHeader + bufferIndex, length);
            bufferIndex += length;
        }
    }
    else
    {
        //read data from file;
        fread(buffer, sizeof(uint8_t), length, in);
    }
}

bool BMPBox::writeToFile(std::string fileName)
{

    int row;
    unsigned char *buf;

    out = NULL;
    out = fopen(fileName.c_str(), "w");
    if (out == NULL)
    {
        fprintf(stderr, "[ debug ] Error: Could not write to file %s!\n", fileName.c_str());
        return false;
    }

    /* Write the file */
    writeHeader(header);
    writeInfo(info);
    writePalette(palette);
    double bytes_per_pixel;
    int bytes_per_line;

    bytes_per_pixel = (bitDepth * 1.0) / 8.0;
    bytes_per_line = (int)ceil(bytes_per_pixel * width);
    if (bytes_per_line % 4 != 0)
    {
        bytes_per_line += 4 - bytes_per_line % 4;
    }

    buf = (unsigned char *)malloc(bytes_per_line);

    for (row = height - 1; row >= 0; --row)
    {
        memset(buf, 0, bytes_per_line);
        fillBufferFromRow(buf, bytes_per_line);
        fwrite(buf, sizeof(uint8_t), bytes_per_line, out);
        fflush(out);
    }
    free(buf);

    return true;
}

BMPBox::~BMPBox()
{
    fclose(in); in = NULL;
    fclose(out); out = NULL;

    if (fileHeader != NULL) {
        free(fileHeader);
        fileHeader = NULL;
    }
    if (header != NULL) {
        free(header);
        header = NULL;
    }

    if (info != NULL) {
        free(info);
        info = NULL;
    }
    if (palette != NULL) {
        free(palette);
        palette = NULL;
    }
    fprintf(stderr, "#######################################################################\n");
}
