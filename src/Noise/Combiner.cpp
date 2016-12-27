#include "Noise/Combiner.h"
#include <cassert>

using namespace noise::nodes;

Combiner::Combiner(const Node *input1, const Node *input2) : input1(input1), input2(input2)
{

}

Add::Add(const Node *input1, const Node *input2, float strength ) : Combiner(input1, input2), strength(strength)
{
}

float Add::operator()(float x, float y, float z) const
{
    assert(input1 && input2);
    return (*input1)(x, y, z) + (*input2)(x, y, z) * strength;
}

Multiply::Multiply(const Node *input1, const Node *input2, float strength ) : Combiner(input1, input2), strength(strength)
{
}

float Multiply::operator()(float x, float y, float z) const
{
    assert(input1 && input2);
    return (*input1)(x, y, z) * (*input2)(x, y, z) * strength;
}