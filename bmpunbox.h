#ifndef BMPUNBOX_H
#define BMPUNBOX_H

#include <string>

#include "bmp.h"
#include "fileinfo.h"

class BMPUnbox
{

private:
    FILE* in;
    geninfo_t* fileInfo;

public:
    /**
     * @brief BMPUnbox Unbox a general file from a BMP image.
     * @param fileName image name.
     */
    BMPUnbox(std::string fileName);

    /**
     * @brief getFileInfo Gets a pointer to fileinfo structure.
     * @return a pointer to a geninfo_t.
     */
    geninfo_t* getFileInfo();

    /**
     * @brief readBytes Reads a number of length unsigned 8 bit values from the input file.
     * @param buffer Pointer to a block of memory with a width of at least length.
     * @param length The number of bytes to read.
     * @return The number of bytes that could be read.
     */
    size_t readBytes(uint8_t* buffer, uint32_t length);
    ~BMPUnbox();
};

#endif // BMPUNBOX_H
