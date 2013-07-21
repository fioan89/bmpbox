#ifndef BMPBOX_H
#define BMPBOX_H

#include <string>
#include "bmp.h"

class BMPBox
{
private:
    uint8_t bit_depth;
    uint32_t nr_of_colors;
    uint32_t width;
    uint32_t height;
    uint32_t aditionalBytes;

    bmpheader_t header;
    bmpinfo_t info;
    rgba_t** pixels;
    rgba_t* palette;

    FILE* in;
    FILE* out;

    size_t getFileSzInBytes(FILE* fp);
    uint32_t* findTwoDivisors(uint32_t number);

public:
    /**
     * @brief bmpbox Build a BMP image from a generic file.
     * @param fileName name of the generic file.
     */
    BMPBox(std::string fileName);

    /**
     * @brief getAditionalBytes returns the number of dummy bytes inserted
     * at the end of the image in order to keep a good aspec ratio.
     * @return the number of aditional dummy bytes inserted in the picture.
     */
    uint32_t getAditionalBytes();

    /**
     * @brief getBitDepth returns image bit depth. Always should be 8.
     * @return image bit depth.
     */
    uint8_t getBitDepth();

    /**
     * @brief getNumberOfColors returns the number of colors on a palette.
     * @return number of colors.
     */
    uint32_t getNumberOfColors();

    /**
     * @brief allocateColorTable creates a buffer for storing palette colors.
     * @return a pointer to a buffer.
     */

    rgba_t* allocateColorTable();
    /**
     * @brief genColorTable fills a buffer with a grayscale color table.
     * @param colors an rgba buffer.
     */
    void genColorTable(rgba_t *colors);

    /**
     * @brief allocatePixels allocate pixel buffer.
     * @return a pointer to a buffer.
     */
    rgba_t** allocatePixels();

    /**
     * @brief setPixel pretty obvious, sets the value if a rgba pixel.
     * @param x index on the x axis (column).
     * @param y index on the y axis (row)
     * @param pixel rgba value.
     * @return true if x or y is not greater than width or height, false otherwise.
     */
    bool setPixel(uint32_t x, uint32_t y, rgba_t pixel);
    /**
     * @brief isBigEndian Checks if current architecture is big endian or not.
     * @return true if big endian, false otherwise.
     */
    bool isBigEndian();

    /**
     * @brief swapHeaderEndianess If big endian than swap header to little endian.
     * @param header bmp header encoded in big endian.
     */
    void swapHeaderEndianess(bmpheader_t* header);

    /**
     * @brief swapInfoEndianess If big endian than swap info structure to little endian.
     * @param info bmp info structure encoded in big endian.
     */
    void swapInfoEndianess(bmpinfo_t* info);

    /**
     * @brief writeHeader writes information encoded in the header structure to a file.
     * File pointer is specified by the writeToFile who most of the times calls this method.
     * @param header pointer to a bmp header structure.
     */
    void writeHeader(bmpheader_t* header);

    /**
     * @brief writeInfo writes information encoded in the info structure to a file.
     * File pointer is specified by the writeToFile who most of the times calls this method.
     * @param info pointer to a bmp info structure.
     */
    void writeInfo(bmpinfo_t* info);

    /**
     * @brief writePalette Fills an rgba buffer with a grayscale palette-.
     * @param palette pointer to an rgba buffer.
     */
    void writePalette(rgba_t* palette);

    /**
     * @brief fillBufferFromRow Fills a buffer with data.
     * @param buffer 8 bit buffer where data will be stored.
     * @param length buffer length. Must be equal with image width.
     * @param row Tells from which row of the general
     * file buffer should be filled.
     */
    void fillBufferFromRow(uint8_t* buffer, size_t length, uint32_t row);

    /**
     * @brief writeToFile Saves the general file to BMP image file.
     * @param fileName location where image should be saved.
     * @return true if saving job succeeded, false otherwise.
     */
    bool writeToFile(std::string fileName);
    ~BMPBox();

};

#endif // BMPBOX_H
