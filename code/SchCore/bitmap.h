#ifndef __BITMAP_H_
#define __BITMAP_H_

#ifdef __BITMAP_C_
#define BITMAP_EXT
#else
#define BITMAP_EXT extern
#endif

#include <stdint.h>

typedef struct
{
    uint32_t bitmap;
}BitMap_t;

BITMAP_EXT void     BitmapInit          (BitMap_t *bitmap);
BITMAP_EXT uint32_t BitmapPosCount      (void);
BITMAP_EXT void     BitmapSet           (BitMap_t *bitmap, uint32_t pos);
BITMAP_EXT void     BitmapClear         (BitMap_t *bitmap, uint32_t pos);
BITMAP_EXT uint32_t BitmapGetFirstSet   (BitMap_t *bitmap);

#endif /* __BITMAP_H_ */
