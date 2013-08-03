#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "bmpunbox.h"

BMPUnbox::BMPUnbox(std::string fileName)
{
    unsigned int i = 0;
    unsigned int paletteSz = pow(2, BITDEPTH) * 4;
    if (fileName.find_last_of(".bmp") == std::string::npos)
    {
        fprintf(stderr, "[ debug ] Error: file %s does not look like a valid BMP image. File extension is not .bmp!\n", fileName.c_str());
        return;
    }
    in = NULL;
    in = fopen(fileName.c_str(), "r");
    if (in == NULL)
    {
        fprintf(stderr, "[ debug ] Error: file %s could not be opened!\n", fileName.c_str());
        return;
    }

    fprintf(stderr, "[ debug ] Info: seeking over %d bytes!\n", 14 + 40 + paletteSz);
    fseek(in, 14 + 40 + paletteSz, SEEK_SET);
    // read file info
    fileInfo = NULL;
    fileInfo = (geninfo_t*) malloc(sizeof(geninfo_t));
    // read magic
    fread(&(fileInfo->magic[0]), sizeof(uint8_t), 1, in);
    fread(&(fileInfo->magic[1]), sizeof(uint8_t), 1, in);
    fread(&(fileInfo->magic[2]), sizeof(uint8_t), 1, in);
    // read version
    fread(&(fileInfo->major_version), sizeof(uint8_t), 1, in);
    fread(&(fileInfo->minor_version), sizeof(uint8_t), 1, in);
    // read full name
    for (i = 0; i < 1024; i++)
    {
        fread(&(fileInfo->full_name[i]), sizeof(char), 1, in);
    }
    // read name
    for (i = 0; i < 256; i++)
    {
        fread(&(fileInfo->file_name[i]), sizeof(char), 1, in);
    }
    // read directory path
    for (i = 0; i < 1024; i++)
    {
        fread(&(fileInfo->parent_directory[i]), sizeof(char), 1, in);
    }
    // read extension
    for (i = 0; i < 255; i++)
    {
        fread(&(fileInfo->extension[i]), sizeof(char), 1, in);
    }
    //
    fread(&(fileInfo->data_size), sizeof(uint32_t), 1, in);
    fread(&(fileInfo->is_chunk), sizeof(uint8_t), 1, in);
    fread(&(fileInfo->is_public), sizeof(uint8_t), 1, in);

}

geninfo_t* BMPUnbox::getFileInfo()
{
    geninfo_t* clone = (geninfo_t*) malloc(sizeof(geninfo_t));
    if (clone != NULL && fileInfo != NULL)
    {
        clone->magic[0] = fileInfo->magic[0];
        clone->magic[1] = fileInfo->magic[1];
        clone->magic[2] = fileInfo->magic[2];
        clone->major_version = fileInfo->major_version;
        clone->minor_version = fileInfo->minor_version;
        strncpy(clone->full_name, fileInfo->full_name, 1024);
        strncpy(clone->file_name, fileInfo->file_name, 256);
        strncpy(clone->parent_directory, fileInfo->parent_directory, 1024);
        strncpy(clone->extension, fileInfo->extension, 255);
        clone->data_size = fileInfo->data_size;
        clone->is_public = fileInfo->is_public;
    }
    return clone;
}

size_t BMPUnbox::readBytes(uint8_t *buffer, uint32_t length)
{
    return fread(buffer, sizeof(uint8_t), length, in);
}


BMPUnbox::~BMPUnbox()
{
    if (in)
    {
        fclose(in);
        in = NULL;
    }
    if (fileInfo != NULL)
    {
        free(fileInfo);
        fileInfo = NULL;
    }
}
