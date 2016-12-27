#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include "Heightmap.h"
#include "Bitmap.h"
#include <cassert>
#include "Noise.h"

using namespace noise::nodes;

int main(int argc, char *argv[])
{
    Perlin perlin1(3, 3);
    Perlin perlin2(1, 2);
    perlin1.Seed(2);
    Constant constant(0);

    Add add(&perlin2, &constant);
    Multiply multiply(&perlin1, &add);

    return 0;
}