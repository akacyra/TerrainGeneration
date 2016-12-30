#include "Node.h"
#include <cmath>
#include <cstring>
#include "NodeRenderer.h"
#include "lodepng.h"

int Node::idCounter = 0;

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
}

void Node::DisconnectInputSlot(unsigned slotNum)
{
    Slot &slot = inputSlots.at(slotNum);
    slot.toNode->outputSlots.at(slot.toSlot) = Slot();
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
    return ImVec2(pos.x + size.x, pos.y + size.y * ((float)slotNum + 1) / ((float)outputCount + 1)); 
}

float Perlin::Evaluate(float x, float y, float z) const
{
    return noise.Sample(x, y, z, octaves, frequency, persistence, lacunarity);
}

void Perlin::DrawControls()
{
    if (ImGui::SliderInt("##seed", (int *)&seed, 0, std::numeric_limits<int>::max() - 1, "Seed %.0f")) {
        noise.Seed(seed);
    }
    ImGui::SliderInt("##octaves", (int *)&octaves, 1, 10, "Octaves %.0f");
    ImGui::SliderFloat("##frequency", &frequency, 0.0f, 64.0f, "Frequency %.3f");
    ImGui::SliderFloat("##persistence", &persistence, 0.0f, 64.0f, "Persistence %.3f");
    ImGui::SliderFloat("##lacunarity", &lacunarity, 0.0f, 64.0f, "Lacunarity %.3f");
}

void Perlin::Reset()
{
    seed = 0;
    octaves = 3;
    frequency = 1.0f;
    persistence = 0.5f;
    lacunarity = 2.0f;
}

float Voronoi::Evaluate(float x, float y, float z) const
{
    return noise.Sample(x, y, z, frequency);
}

void Voronoi::DrawControls()
{
    if (ImGui::SliderInt("##seed", (int *)&seed, 0, std::numeric_limits<int>::max() - 1, "Seed %.0f")) {
        noise.Seed(seed);
    }
    ImGui::SliderFloat("##frequency", &frequency, 0.0f, 64.0f, "Frequency %.3f");
}

void Voronoi::Reset()
{
    seed = 0;
    frequency = 1.0f;
}

float Constant::Evaluate(float x, float y, float z) const
{
    return value;
}

void Constant::DrawControls()
{
    ImGui::SliderFloat("##value", &value, 0.0f, 1.0f, "Value %.3f");
}

void Constant::Reset()
{
    value = 0.0f;
}


float Abs::Evaluate(float x, float y, float z) const
{
    const Node *in = InputSlot(0).toNode;

    return  in ? fabs(in->Evaluate(x, y, z)) : 0.0f;
}

float Invert::Evaluate(float x, float y, float z) const
{
    const Node *in = InputSlot(0).toNode;

    return  in ? -in->Evaluate(x, y, z) : 0.0f;
}

float Combine::Evaluate(float x, float y, float z) const
{
    const Node *in1 = InputSlot(0).toNode;
    const Node *in2 = InputSlot(1).toNode;

    return  func((in1 ? in1->Evaluate(x, y, z) : 0.0f), (in2 ? in2->Evaluate(x, y, z) * strength : 0.0f));
}

const char *combineComboItems[] = {
    "Add", "Multiply"
};

void Combine::DrawControls()
{
    ImGui::SliderFloat("##strength", &strength, 0.0f, 1.0f, "Strength %.3f");

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

void ImageOutput::DrawControls()
{
    ImGui::InputText("Filename", buffer, 128);
    ImGui::SliderInt("Image Size", (int *)&imageSize, 1, 8192, "%.0f");
    if (ImGui::Button("Save")) {
        NodeRenderer renderer(imageSize);
        const NodeRenderer::ImageData image = renderer.Render(this);
        lodepng::encode(std::string(buffer) + ".png", image, imageSize, imageSize, LCT_RGB, 8);
    }
}

float Splitter::Evaluate(float x, float y, float z) const
{
    const Node *in = InputSlot(0).toNode;

    return in ? in->Evaluate(x, y, z) : 0.0f;
}

void Splitter::DrawControls()
{
    if (ImGui::SliderInt("##", &count, 2, 10, "%.0f")) {
        OutputCount(count);
    }
    ImGui::Dummy(ImVec2(0.0f, 16.0f * (OutputCount() - 2)));
}