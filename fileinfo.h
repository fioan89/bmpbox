#ifndef FILEINFO_H
#define FILEINFO_H

#include "bmptypes.h"

#define MAJOR_VERSION 1
#define MINOR_VERSION 0

#pragma pack(1)
typedef struct {
    uint8_t magic[3];        /* The magic letters to idenntify that this file is
                                a general file encoded to BMP image.
                                The following entries are posible:
                                GFE - general file encoded.*/
    uint8_t major_version;   /* Major version for this header */
    uint8_t minor_version;   /* Minor version for this header */
    char full_name[1024];    /* The full, path plus the name of the file. Ussualy on Windows and
                                Linux fileName length is limited to 256 bytes, but the
                                path name can be unlimited. */
    char file_name[256];     /* File name with extension */
    char parent_directory[1024]; /* The folder name where this file is stored. */
    char extension[255];     /* File extension */
    uint32_t data_size;      /* How much of the data is actual general file body.*/
    uint8_t is_chunk;        /* 0 if this file is not splitted across multiple BMP files,
                                a value greater than 1 otherwise. This is in order to store big files. */
    uint8_t is_public;       /* 0 if this file is a private file that only the user can see, or a greater
                               value if this file can be shared with the public. */
} geninfo_t;
#pragma pack()
#endif // FILEINFO_H
