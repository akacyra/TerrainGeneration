#include "Node.h"
#include <cstring>
#include "NodeRenderer.h"
#include "lodepng.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

int Node::idCounter = 0;

float clamp(float v) { return (v < 0.0f) ? 0.0f : (v > 1.0f) ? 1.0f : v; }

Node::Node(unsigned inputCount, unsigned outputCount, std::string name) : inputCount(inputCount), outputCount(outputCount), inputSlots(inputCount), outputSlots(outputCount), name(name), id(idCounter++) 
{
};

Node::Node(const Node &other)
{
    pos = other.pos;
    size = other.size;
    name = other.name;
    inputCount = other.inputCount;
    outputCount = other.outputCount;
    inputSlots = std::vector<Slot>(inputCount);
    outputSlots = std::vector<Slot>(outputCount);

    id = idCounter++;
}

unsigned Node::InputCount() const 
{ 
    return inputCount; 
};

unsigned Node::OutputCount() const 
{ 
    return outputCount; 
};

const char *Node::Name() const 
{ 
    return name.c_str(); 
};

int Node::ID() const 
{ 
    return id; 
};

bool Node::IsInputSlotConnected(unsigned slotNum) const
{
    return InputSlot(slotNum).toNode != nullptr;
}

bool Node::IsOutputSlotConnected(unsigned slotNum) const
{
    return OutputSlot(slotNum).toNode != nullptr;
}

void Node::ConnectInputSlot(unsigned thisSlot, Node *toNode, unsigned toSlot)
{
    inputSlots.at(thisSlot) = Slot(toNode, toSlot);
    toNode->outputSlots.at(toSlot) = Slot(this, thisSlot);

    if (toSlot == toNode->OutputCount() - 1) {
        toNode->OutputCount(toNode->OutputCount() + 1);
    }
}

void Node::DisconnectInputSlot(unsigned slotNum)
{
    Slot &slot = inputSlots.at(slotNum);
    Node *output = slot.toNode;

    output->outputSlots[slot.toSlot] = output->outputSlots[output->outputCount - 2];
    output->outputSlots[output->outputCount - 2] = Slot();
    output->outputCount--;

    slot = Slot();
}

void Node::ConnectOutputSlot(unsigned thisSlot, Node *toNode, unsigned toSlot) 
{
    toNode->ConnectInputSlot(toSlot, this, thisSlot);
}

void Node::DisconnectOutputSlot(unsigned slotNum) 
{
    OutputSlot(slotNum).toNode->DisconnectInputSlot(OutputSlot(slotNum).toSlot);
}

void Node::DisconnectAll() 
{
    for (unsigned i = 0; i < inputCount; i++) {
        if (IsInputSlotConnected(i)) {
            DisconnectInputSlot(i);
        }
    }
    for (unsigned i = 0; i < outputCount; i++) {
        if (IsOutputSlotConnected(i)) {
            DisconnectOutputSlot(i);
        }
    }
}

Slot Node::InputSlot(unsigned slotNum) const 
{ 
    return inputSlots.at(slotNum); 
}

Slot Node::OutputSlot(unsigned slotNum) const 
{ 
    return outputSlots.at(slotNum); 
};

void Node::InputCount(unsigned count)
{
    for (unsigned i = count; i < inputCount; i++) {
        if (IsInputSlotConnected(i)) {
            DisconnectInputSlot(i);
        }
    }
    inputCount = count;
    inputSlots.resize(count);
}

void Node::OutputCount(unsigned count)
{
    for (unsigned i = count; i < outputCount; i++) {
        if (IsOutputSlotConnected(i)) {
            DisconnectOutputSlot(i);
        }
    }
    outputCount = count;
    outputSlots.resize(count);
}

ImVec2 Node::InputSlotPos(unsigned slotNum) const 
{ 
    return ImVec2(pos.x, pos.y + size.y * ((float)slotNum + 1) / ((float)inputCount + 1)); 
}

ImVec2 Node::OutputSlotPos(unsigned slotNum) const 
{ 
    return ImVec2(pos.x + size.x, pos.y + size.y * 0.5f);
}

float Perlin::Evaluate(float x, float y, float z) const
{
    float p = noise.Sample(x, y, z, octaves, frequency, persistence, lacunarity);
    return style((p + 1.0f) / 2.0f);
}

const char *perlinComboItems[] = {
    "Classic", "Billowy", "Ridged"
};

void Perlin::DrawControls(ImDrawList *drawList)
{
    if (ImGui::SliderInt("##seed", (int *)&seed, 0, std::numeric_limits<int>::max() - 1, "Seed %.0f")) {
        noise.Seed(seed);
    }
    ImGui::SliderInt("##octaves", (int *)&octaves, 1, 10, "Octaves %.0f");
    ImGui::SliderFloat("##frequency", &frequency, 0.0f, 64.0f, "Frequency %.3f");
    ImGui::SliderFloat("##persistence", &persistence, 0.0f, 8.0f, "Persistence %.3f");
    ImGui::SliderFloat("##lacunarity", &lacunarity, 0.0f, 8.0f, "Lacunarity %.3f");

    if ((ImGui::Combo("##style", &currentStyleIdx, perlinComboItems, 3))) {
        switch(currentStyleIdx) {
            case 0: style = Perlin::Classic; break;
            case 1: style = Perlin::Billowy; break;
            case 2: style = Perlin::Ridged; break;
        }
    }
}

void Perlin::Reset()
{
    seed = 0;
    octaves = 3;
    frequency = 1.0f;
    persistence = 0.5f;
    lacunarity = 2.0f;
    style = Perlin::Classic;
    currentStyleIdx = 0;
}


float Constant::Evaluate(float x, float y, float z) const
{
    return value;
}

void Constant::DrawControls(ImDrawList *drawList)
{
    ImGui::SliderFloat("##value", &value, 0.0f, 1.0f, "Value %.3f");
}

void Constant::Reset()
{
    value = 0.0f;
}

float Gradient::Evaluate(float x, float y, float z) const
{
    return 0.0f;
}

void Gradient::DrawControls(ImDrawList *drawList)
{
    ImGui::DragFloat2("Start", (float *)&start, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat2("End", (float *)&end, 0.1f, 0.0f, 1.0f);

    float size = 100.0f;
    ImVec2 min = ImGui::GetCursorScreenPos();
    ImVec2 max = min + ImVec2(size, size);

    drawList->AddRectFilled(min, max, ImColor(255, 255, 255));
    drawList->AddCircleFilled(min + start * size, 2.0f, ImColor(255, 0, 0));
    drawList->AddCircleFilled(min + end * size, 2.0f, ImColor(255, 0, 0));

    ImGui::Dummy(ImVec2(size, size));
}

void Gradient::Reset()
{
    start = ImVec2(0, 0);
    end = ImVec2(1, 1);
}


float Abs::Evaluate(float x, float y, float z) const
{
    const Node *in = InputSlot(0).toNode;

    if (in) {
        float v = in->Evaluate(x, y, z);
        return fabs(v + -0.5f) + 0.5f;
    } 
    return 0.0f;
}

float Invert::Evaluate(float x, float y, float z) const
{
    const Node *in = InputSlot(0).toNode;

    return  in ? 1.0f - in->Evaluate(x, y, z) : 0.0f;
}

void Selector::Reset()
{
    min = 0.0f;
    max = 1.0f;
    falloff = 1.0f;
}

void Selector::DrawControls(ImDrawList *drawList)
{
    ImGui::DragFloatRange2("##range", &min, &max, 0.01, 0.0f, 1.0f);
    ImGui::SliderFloat("##falloff", &falloff, 0.0f, 1.0f, "Falloff %.3f");
}

float Selector::Evaluate(float x, float y, float z) const
{
    Node *in = InputSlot(0).toNode;

    if (in) {
        float v = in->Evaluate(x, y, z);
        float fuzz = (max - min) * (1.0f - falloff);
        if (v < min) {
            float d = min - v;
            if (d <= fuzz) {
                return 1.0f - d / fuzz;
            }
            return 0.0f;
        } else if (v > max) {
            float d = v - max;
            if (d <= fuzz) {
                return 1.0f - d / fuzz;
            }
            return 0.0f;
        } else {
            return 1.0f;
        }
    }
    return 0.0f;
}

float Combine::Evaluate(float x, float y, float z) const
{
    const Node *in1 = InputSlot(0).toNode;
    const Node *in2 = InputSlot(1).toNode;

    return  clamp(func((in1 ? in1->Evaluate(x, y, z) : 0.0f), (in2 ? in2->Evaluate(x, y, z) * strength : 0.0f)));
}

const char *combineComboItems[] = {
    "Add", "Multiply"
};

void Combine::DrawControls(ImDrawList *drawList)
{
    ImGui::SliderFloat("##strength", &strength, 0.0f, 2.0f, "Strength %.3f");

    if (ImGui::Combo("##function", &currentFuncIdx, combineComboItems, 2)) {
        switch (currentFuncIdx) {
            case 0: func = Combine::Add; break;
            case 1: func = Combine::Multiply; break;
        }
    }
}

void Combine::Reset()
{
    strength = 1.0f;
    func = Combine::Add;
    currentFuncIdx = 0;
}

float ImageOutput::Evaluate(float x, float y, float z) const
{
    Node *in = InputSlot(0).toNode;
    return in ? in->Evaluate(x, y, z) : 0.0f;
}

void ImageOutput::Reset()
{
    memset(buffer, 0, 128);
    buffer[0] = '\0';
    imageSize = 512;
}

void ImageOutput::DrawControls(ImDrawList *drawList)
{
    ImGui::InputText("Filename", buffer, 128);
    ImGui::SliderInt("Image Size", (int *)&imageSize, 1, 8192, "%.0f");
    if (ImGui::Button("Save")) {
        NodeRenderer renderer(imageSize);
        const NodeRenderer::ImageData image = renderer.Render(this);
        lodepng::encode(std::string(buffer) + ".png", image, imageSize, imageSize, LCT_RGB, 8);
    }
}