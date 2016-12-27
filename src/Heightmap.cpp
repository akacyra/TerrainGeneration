#include "Heightmap.h"
#include <algorithm>
#include <cmath>
#include <functional>

Heightmap &Heightmap::add(const Heightmap &other)
{
    assert(Width() == other.Width() && Height() == other.Height());
    std::transform(begin(), end(), other.begin(), begin(), std::plus<float>());
    return *this;
}

Heightmap &Heightmap::multiply(const Heightmap &other)
{
    assert(Width() == other.Width() && Height() == other.Height());
    std::transform(begin(), end(), other.begin(), begin(), std::multiplies<float>());
    return *this;
}

Heightmap &Heightmap::abs()
{
    // TODO: heights [0, 1], so how to handle abs?
    std::transform(begin(), end(), begin(), [](float v) { return v < 0.5 ? 1 - v : v; });
    return *this;
}

Heightmap &Heightmap::invert()
{
    std::transform(begin(), end(), begin(), [](float v) { return 1 - v; });
    return *this;
}

Heightmap &Heightmap::normalize()
{
    float min, max;
    min = max = (*this)(0, 0);
    for (float v : *this) {
        min = std::min(min, v);
        max = std::max(max, v);
    }
    float range = max - min;
    std::transform(begin(), end(), begin(), [=](float v) { return (v - min) / range; });
    return *this;
}