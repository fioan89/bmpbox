#ifndef BMPTYPES_H
#define BMPTYPES_H

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

#ifndef uint16_t
typedef unsigned short uint16_t;
#endif

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#define UINT16_SWAP_ENDIANESS(val)         \
    ((uint16_t)                                   \
    (                                            \
    (uint16_t) ((uint16_t) (val) >> 8) |        \
    (uint16_t) ((uint16_t) (val) << 8)))

#define UINT32_SWAP_ENDIANESS(val)                   \
    ((uint32_t)                                             \
    (                                                      \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#endif // BMPTYPES_H
