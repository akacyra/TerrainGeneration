#ifndef __UI_H__
#define __UI_H__

#include "Workspace.h"
#include "NodeRenderer.h"
#include <GL/glew.h> 

// Adapted from the node graph example by Ocornut: https://gist.github.com/ocornut/7e9b3ec566a333d725d4
void ShowNodeGraphEditor(bool *opened, Workspace &workspace, NodeRenderer &renderer, GLuint previewTextureID);

#endif