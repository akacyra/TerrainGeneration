#ifndef __BITMAP_H__
#define __BITMAP_H__

#include "Array2D.h"
#include <cstdint>

typedef uint8_t Gray8;
typedef uint16_t Gray16;

struct RGB24 
{
    uint8_t r, g, b;
};

struct RGBA32 
{
    uint8_t r, g, b, a;
};


template<typename ColorType>
class Bitmap : public Array2D<ColorType>
{
    public:
        typedef typename Array2D<ColorType>::size_type size_type;

        Bitmap(size_type width, size_type height) : Array2D<ColorType>(width, height) { };
        Bitmap(size_type width, size_type height, ColorType c) : Array2D<ColorType>(width, height, c) { };
    
    private:
};

typedef Bitmap<Gray8> BitmapGray8;
typedef Bitmap<Gray16> BitmapGray16;
typedef Bitmap<RGB24> BitmapRGB24;
typedef Bitmap<RGBA32> BitmapRGBA32;

unsigned encodeToFile(const char *filename, const BitmapGray8 &bitmap);
unsigned encodeToFile(const char *filename, const BitmapGray16 &bitmap);
unsigned encodeToFile(const char *filename, const BitmapRGB24 &bitmap);
unsigned encodeToFile(const char *filename, const BitmapRGBA32 &bitmap);

#endif