#ifndef __VORONOI_NOISE_H__
#define __VORONOI_NOISE_H__

#include <cstdint> 

namespace noise {

    class VoronoiNoise
    {
        public:
            VoronoiNoise(uint64_t seed);

            float Sample(float x, float y, float z) const;
            float Sample(float x, float y, float z, float frequency) const;

            void Seed(uint64_t seed);

            typedef float (*DistanceFunc)(float, float, float);

            static float Euclidean(float dx, float dy, float dz);
            static float Manhattan(float dx, float dy, float dz);
            static float Chebyshev(float dx, float dy, float dz);   

            DistanceFunc distance = &Euclidean;

        private:
            unsigned permutation[256];

            unsigned Hash(unsigned x, unsigned y, unsigned z) const;

    };

}

#endif