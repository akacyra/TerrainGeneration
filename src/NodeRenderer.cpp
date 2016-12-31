#include "NodeRenderer.h"

const NodeRenderer::ImageData &NodeRenderer::Render(const Node *node)
{
    image.clear();

    for (unsigned i = 0; i < imageSize; i++) {
        for(unsigned j = 0; j < imageSize; j++) {
            float v = node->Evaluate((float)j / imageSize, (float)i / imageSize, 0.0f);
            unsigned char b = v * 255;
            image.insert(image.end(), { b, b, b });
        }
    }

    return image;
}