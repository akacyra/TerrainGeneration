#ifndef __FILTER_H__
#define __FILTER_H__

#include "noise/Node.h"

namespace noise { namespace filter {

    class Filter : public Node
    {
        public:
            Filter(const Node *input);

            const Node *input;
    };

    class Abs : public Filter
    {
        public:
            Abs(const Node *input = nullptr);
        
            float operator()(float x, float y, float z) const;
    };

    class Invert : public Filter
    {
        public:
            Invert(const Node *input = nullptr);

            float operator()(float x, float y, float z) const;
    };

    class Clamp : public Filter
    {
        public:
            Clamp(const Node *input = nullptr, float min = 0.0, float max = 1.0);

            float operator()(float x, float y, float z) const;

            float min;
            float max;
    };

}}

#endif