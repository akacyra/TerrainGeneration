#include "Noise/Filter.h"
#include <cmath>
#include <cassert>
#include <algorithm>

using namespace noise::filter;

Filter::Filter(const Node *input) : input(input)
{
}

Abs::Abs(const Node *input) : Filter(input)
{
}

float Abs::operator()(float x, float y, float z) const
{
    assert(input);
    return fabs((*input)(x, y, z));
}

Invert::Invert(const Node *input) : Filter(input)
{
}

float Invert::operator()(float x, float y, float z) const
{
    assert(input);
    return -(*input)(x, y, z);
}

Clamp::Clamp(const Node *input, float min, float max) : Filter(input), min(min), max(max)
{
}

float Clamp::operator()(float x, float y, float z) const
{
    assert(input);
    return std::max(min, std::min((*input)(x, y, z), max));
}