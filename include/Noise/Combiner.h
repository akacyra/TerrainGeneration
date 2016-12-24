#ifndef __COMBINER_H__
#define __COMBINER_H__

#include "noise/Node.h"

namespace noise { namespace combiner {

    class Combiner : public Node
    {
        public:
            Combiner(const Node *input1, const Node *input2);

            const Node *input1;
            const Node *input2;
    };

    class Add : public Combiner
    {
        public:
            Add(const Node *input1 = nullptr, const Node *input2 = nullptr, float strength = 1.0);

            float operator()(float x, float y, float z) const;

            float strength;
    };

    class Multiply : public Combiner
    {
        public:
            Multiply(const Node *input1 = nullptr, const Node *input2 = nullptr, float strength = 1.0);

            float operator()(float x, float y, float z) const;

            float strength;
    };
}}

#endif