#ifndef __NODE_RENDERER_H__
#define __NODE_RENDERER_H__

#include <vector>
#include "Node.h"

class NodeRenderer
{
    public:
        typedef std::vector<unsigned char> ImageData;

        NodeRenderer() : NodeRenderer(128) { };
        NodeRenderer(unsigned size) : imageSize(size), image(size * size * 3) { };

        const ImageData &Render(const Node *node);

        unsigned ImageSize() const { return imageSize; };
        void ImageSize(unsigned size) { imageSize = size; };

    private:
        unsigned imageSize;
        ImageData image;
};

#endif