#include "Noise/VoronoiNoise.h"
#include <cassert>
#include <random>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace noise;

unsigned numPointsLookup(unsigned x)
{
    if (x < 23) return 1;
    if (x < 61) return 2;
    if (x < 110) return 3;
    if (x < 161) return 4;
    if (x < 201) return 5;
    if (x < 228) return 6;
    if (x < 243) return 7;
    if (x < 251) return 8;
    return 9;
}

unsigned VoronoiNoise::Hash(unsigned x, unsigned y, unsigned z) const
{
    unsigned a = permutation[x & 255] + y;
    unsigned b = permutation[a & 255] + z;
    return permutation[b & 255];
}

VoronoiNoise::VoronoiNoise(uint64_t seed)
{
    for (unsigned i = 0; i < 256; i++) {
        permutation[i] = i;
    }

    Seed(seed);
}

float VoronoiNoise::Euclidean(float dx, float dy, float dz)
{
    return dx * dx + dy * dy + dz * dz;
}

float VoronoiNoise::Manhattan(float dx, float dy, float dz)
{
    return fabs(dx) + fabs(dy) + fabs(dz);
}

float VoronoiNoise::Chebyshev(float dx, float dy, float dz)
{
    return std::max({ fabs(dx), fabs(dy), fabs(dz) });
}

float VoronoiNoise::Sample(float x, float y, float z) const
{
    assert(x >= 0 && y >= 0);

    unsigned xCube = (unsigned)x & 255,
             yCube = (unsigned)y & 255,
             zCube = (unsigned)z & 255;

    float minDist = std::numeric_limits<float>::max();

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; k++) {
                unsigned xCurCube = xCube + i;
                unsigned yCurCube = yCube + j;
                unsigned zCurCube = zCube + k;

                std::minstd_rand prng(Hash(xCurCube, yCurCube, zCurCube));
                unsigned numPoints = numPointsLookup(prng());

                for (unsigned l = 0; l < numPoints; l++) {
                    float xRel = (float)prng() / std::minstd_rand::max();
                    float yRel = (float)prng() / std::minstd_rand::max();
                    float zRel = (float)prng() / std::minstd_rand::max();

                    float d = distance(xCurCube + xRel - x, yCurCube + yRel - y, zCurCube + zRel - z);

                    minDist = std::min(minDist, d);
                }
            }
        }
    }

    return minDist;
}

float VoronoiNoise::Sample(float x, float y, float z, float frequency) const
{
    return Sample(x * frequency, y * frequency, z * frequency);
}

void VoronoiNoise::Seed(uint64_t seed)
{
    std::mt19937_64 prng(seed);

    std::shuffle(permutation, permutation + 256, prng);
}