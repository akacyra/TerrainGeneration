#ifndef __NODE_H__
#define __NODE_H__

namespace noise { namespace nodes {

    class Node 
    {
        public:
            virtual ~Node() { };

            virtual float operator()(float x, float y, float z) const = 0;
    };

}}

#endif