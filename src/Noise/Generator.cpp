#include "Noise/Generator.h"
#include <cmath>

using namespace noise::nodes;

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

Ridged::Ridged(unsigned octaves, float frequency, float persistence, float lacunarity) 
    : Perlin(octaves, frequency, persistence, lacunarity)
{
}

float Ridged::operator()(float x, float y, float z) const
{
    return 1 - fabs(noise.Sample(x, y, z, octaves, frequency, persistence, lacunarity));
}

Billows::Billows(unsigned octaves, float frequency, float persistence, float lacunarity) 
    : Perlin(octaves, frequency, persistence, lacunarity)
{
}

float Billows::operator()(float x, float y, float z) const
{
    return fabs(noise.Sample(x, y, z, octaves, frequency, persistence, lacunarity));
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