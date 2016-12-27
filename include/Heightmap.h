#ifndef __HEIGHTMAP_H__
#define __HEIGHTMAP_H__

#include "Array2D.h"

class Heightmap : public Array2D<float>
{
    public:

        Heightmap()
        {
        }

        Heightmap(size_type width, size_type height) : Array2D<float>(width, height)
        {
        }

        Heightmap(size_type width, size_type height, float val) : Array2D<float>(width, height, val)
        {
        }
        
        Heightmap &add(const Heightmap &other);
        Heightmap &multiply(const Heightmap &other);
        Heightmap &abs();
        Heightmap &invert();
        Heightmap &normalize();

    private:
};

#endif