#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "noise/Node.h"
#include "PerlinNoise.h"
#include "VoronoiNoise.h"


namespace noise { namespace generator {

    class Generator : public Node
    {
    };

    class Constant : public Generator
    {
        public:
            Constant(float value = 0.0);

            float operator()(float x, float y, float z) const;

            float value;
    };

    class Perlin : public Generator
    {
        public:
            Perlin(unsigned octaves = 3, float frequency = 1.0, float persistence = 0.5, float lacunarity = 2.0);

            float operator()(float x, float y, float z) const;

            void Seed(uint64_t seed);

            unsigned octaves;
            float frequency;
            float persistence;
            float lacunarity;

        private:
            PerlinNoise noise;
    };

    class Voronoi : public Generator
    {
        public:
            Voronoi(float frequency = 1.0);

            float operator()(float x, float y, float z) const;

            void Seed(uint64_t seed);
            void Distance(VoronoiNoise::DistanceFunc distance);

            float frequency;

        private:
            VoronoiNoise noise;
    };

}}

#endif