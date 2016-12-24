#ifndef __PERLIN_NOISE_H__
#define __PERLIN_NOISE_H__

#include <cstdint> 

namespace noise {

    class PerlinNoise
    {
        public:
            PerlinNoise(uint64_t seed);

            float Sample(float x, float y, float z) const;
            float Sample(float x, float y, float z, unsigned octaves, float frequency = 1, float persistence = 0.5, float lacunarity = 2) const;

            void Seed(uint64_t seed);

        private:
            unsigned permutation[256];

            unsigned Hash(unsigned x, unsigned y, unsigned z) const;
            float Grad(unsigned x, unsigned y, unsigned z, float dx, float dy, float dz) const;

    };

}

#endif