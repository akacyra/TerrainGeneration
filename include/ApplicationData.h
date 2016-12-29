#ifndef __APPLICATION_DATA_H__
#define __APPLICATION_DATA_H__

#include "Node.h"
#include <unordered_map>
#include <vector>
#include <GL/glew.h> 

class ApplicationData
{
    public:
        std::unordered_map<int, Node *> nodes;

        GLuint renderTextureID;
        std::vector<unsigned char> renderTextureData;

        unsigned previewImageSize;

        int nodeSelected;
        int slotSelected;
        bool slotIsOutput;

        void Reset() 
        {
            nodes.clear();

            previewImageSize = 128;

            renderTextureData = std::vector<unsigned char>(previewImageSize * previewImageSize * 3, 0);

            nodeSelected = -1;
            slotSelected = -1;
            slotIsOutput = false;
        }

        ApplicationData()
        {
            glGenTextures(1, &renderTextureID);
            Reset();
        }

        ~ApplicationData()
        {
            glDeleteTextures(1, &renderTextureID);
        }
};

#endif