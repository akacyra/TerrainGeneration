#include "Bitmap.h"
#include "lodepng.h"

unsigned encodeToFile(const char *filename, const BitmapGray8 &bitmap) 
{
    return lodepng::encode(std::string(filename) + ".png", (unsigned char *)bitmap.Data(), bitmap.Width(), bitmap.Height(), LCT_GREY, 8);
}

unsigned encodeToFile(const char *filename, const BitmapGray16 &bitmap) 
{
    return lodepng::encode(std::string(filename) + ".png", (unsigned char *)bitmap.Data(), bitmap.Width(), bitmap.Height(), LCT_GREY, 16);
}

unsigned encodeToFile(const char *filename, const BitmapRGB24 &bitmap) 
{
    return lodepng::encode(std::string(filename) + ".png", (unsigned char *)bitmap.Data(), bitmap.Width(), bitmap.Height(), LCT_RGB, 8);
}

unsigned encodeToFile(const char *filename, const BitmapRGBA32 &bitmap) 
{
    return lodepng::encode(std::string(filename) + ".png", (unsigned char *)bitmap.Data(), bitmap.Width(), bitmap.Height(), LCT_RGBA, 8);
}