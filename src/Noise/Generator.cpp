#include "Noise/Generator.h"

using namespace noise::generator;

Constant::Constant(float value) : value(value)
{
}

float Constant::operator()(float x, float y, float z) const
{
    return value;
}

Perlin::Perlin(unsigned octaves, float frequency, float persistence, float lacunarity) 
    : octaves(octaves), frequency(frequency), persistence(persistence), lacunarity(lacunarity), noise(0)
{
}

float Perlin::operator()(float x, float y, float z) const
{
    return noise.Sample(x, y, z, octaves, frequency, persistence, lacunarity);
}

void Perlin::Seed(uint64_t seed)
{
    noise.Seed(seed);
}

Voronoi::Voronoi(float frequency) : frequency(frequency), noise(0)
{
}

float Voronoi::operator()(float x, float y, float z) const
{
    return noise.Sample(x, y, z, frequency);
}

void Voronoi::Seed(uint64_t seed)
{
    noise.Seed(seed);
}

void Voronoi::Distance(VoronoiNoise::DistanceFunc distance)
{
    noise.distance = distance;
}