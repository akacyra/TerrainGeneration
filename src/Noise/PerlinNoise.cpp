#include "Noise/PerlinNoise.h"
#include <cassert>
#include <random>
#include <algorithm>
#include <cmath>

using namespace noise;

unsigned PerlinNoise::Hash(unsigned x, unsigned y, unsigned z) const
{
    unsigned a = permutation[x & 255] + y;
    unsigned b = permutation[a & 255] + z;
    return permutation[b & 255];
}

float PerlinNoise::Grad(unsigned x, unsigned y, unsigned z, float dx, float dy, float dz) const
{
    switch (Hash(x, y, z) & 15) {
        case 0x0: return  dx + dy;
        case 0x1: return -dx + dy;
        case 0x2: return  dx - dy;
        case 0x3: return -dx - dy;
        case 0x4: return  dx + dz;
        case 0x5: return -dx + dz;
        case 0x6: return  dx - dz;
        case 0x7: return -dx - dz;
        case 0x8: return  dy + dz;
        case 0x9: return -dy + dz;
        case 0xA: return  dy - dz;
        case 0xB: return -dy - dz;
        case 0xC: return  dy + dx;
        case 0xD: return -dy + dz;
        case 0xE: return  dy - dx;
        case 0xF: return -dy - dz;
        default: return 0;
    }
}

float Ease(float p)
{
    return p * p * p * (p * (p * 6  - 15) + 10);
}

float Lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

PerlinNoise::PerlinNoise(uint64_t seed)
{
    for (unsigned i = 0; i < 256; i++) {
        permutation[i] = i;
    }

    Seed(seed);
}

float PerlinNoise::Sample(float x, float y, float z) const
{
    assert(x >= 0 && y >= 0);

    unsigned xGrid = (unsigned)x & 255,
             yGrid = (unsigned)y & 255,
             zGrid = (unsigned)z & 255;
    float xRel = x - floor(x),
          yRel = y - floor(y),
          zRel = z - floor(z);
    float u = Ease(xRel),
          v = Ease(yRel),
          w = Ease(zRel);

    return Lerp(w, Lerp(v, Lerp(u, Grad(xGrid    , yGrid    , zGrid    , xRel    , yRel    , zRel   ),
                                   Grad(xGrid + 1, yGrid    , zGrid    , xRel - 1, yRel    , zRel   )),
                           Lerp(u, Grad(xGrid    , yGrid + 1, zGrid    , xRel    , yRel - 1, zRel   ),
                                   Grad(xGrid + 1, yGrid + 1, zGrid    , xRel - 1, yRel - 1, zRel   ))),
                   Lerp(v, Lerp(u, Grad(xGrid    , yGrid    , zGrid + 1, xRel    , yRel    , zRel - 1),
                                   Grad(xGrid + 1, yGrid    , zGrid + 1, xRel - 1, yRel    , zRel - 1)),
                           Lerp(u, Grad(xGrid    , yGrid + 1, zGrid + 1, xRel    , yRel - 1, zRel - 1),
                                   Grad(xGrid + 1, yGrid + 1, zGrid + 1, xRel - 1, yRel - 1, zRel - 1))));
}

float PerlinNoise::Sample(float x, float y, float z, unsigned octaves, float frequency, float persistence, float lacunarity) const
{
    float sum = 0;
    float amplitude = 1;
    float max = 0;
    for (unsigned i = 0; i < octaves; i++) {
        sum += Sample(x * frequency, y * frequency, z * frequency) * amplitude;

        max += amplitude;

        frequency *= lacunarity;
        amplitude *= persistence;
    }
    return sum / max;
}

void PerlinNoise::Seed(uint64_t seed)
{
    std::mt19937_64 prng(seed);

    std::shuffle(permutation, permutation + 256, prng);
}
