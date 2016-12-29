
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
#include "PerlinNoise.h"
#include "VoronoiNoise.h"
#include <limits>

class Node;

struct Slot
{
    Node *toNode;
    int toSlot;

    Slot() : toNode(nullptr), toSlot(-1) { };
    Slot(Node *toNode, int toSlot = -1) : toNode(toNode), toSlot(toSlot) { };
};

class Node
{
    public:

        Node(unsigned inputCount, std::string name) : inputCount(inputCount), inputSlots(inputCount), name(name), id(idCounter++) { };

        virtual ~Node() { };

        unsigned InputCount() const { return inputCount; };

        const char *Name() const { return name.c_str(); };
        
        int ID() const { return id; };

        bool IsInputSlotConnected(unsigned slotNum) const
        {
            return InputSlot(slotNum).toNode != nullptr;
        }

        bool IsOutputSlotConnected() const
        {
            return OutputSlot().toNode != nullptr;
        }

        void ConnectInputSlot(unsigned slotNum, Node *toNode)
        {
            inputSlots.at(slotNum) = Slot(toNode, 0);
            toNode->outputSlot = Slot(this, slotNum);
        }

        void DisconnectInputSlot(unsigned slotNum)
        {
            Slot &slot = inputSlots.at(slotNum);
            slot.toNode->outputSlot = Slot();
            slot = Slot();
        }

        void ConnectOutputSlot(Node *toNode, unsigned toSlot) 
        {
            toNode->ConnectInputSlot(toSlot, this);
        }

        void DisconnectOutputSlot() 
        {
            outputSlot.toNode->DisconnectInputSlot(outputSlot.toSlot);
        }

        void DisconnectAll() 
        {
            for (unsigned i = 0; i < inputCount; i++) {
                if (IsInputSlotConnected(i)) {
                    DisconnectInputSlot(i);
                }
            }
            if (IsOutputSlotConnected()) {
                DisconnectOutputSlot();
            }
        }

        Slot InputSlot(unsigned slotNum) const { return inputSlots.at(slotNum); }

        Slot OutputSlot() const { return outputSlot; };

        ImVec2 InputSlotPos(int slotNo) const 
        { 
            return ImVec2(pos.x, pos.y + size.y * ((float)slotNo + 1) / ((float)inputCount + 1)); 
        }

        ImVec2 OutputSlotPos() const 
        { 
            return ImVec2(pos.x + size.x, pos.y + size.y * 0.5f); 
        }

        virtual float Evaluate(float x, float y, float z) const = 0;

        virtual void DrawControls() = 0;

        ImVec2 pos;
        ImVec2 size;

    private:
        unsigned inputCount;
        std::vector<Slot> inputSlots;

        Slot outputSlot;

        std::string name;

        static int idCounter;
        int id = -1;
};

int Node::idCounter = 0;

// Base class for generators, nodes that generate noise values
class Generator : public Node
{
    public:
        Generator(std::string name) : Node(0, name) { };
};

class Perlin : public Generator
{
    public:
        Perlin() : Generator("Perlin"), noise(0) { };

        float Evaluate(float x, float y, float z) const
        {
            return noise.Sample(x, y, z, octaves, frequency, persistence, lacunarity);
        }

        uint64_t seed = 0;
        unsigned octaves = 3;
        float frequency = 1.0f;
        float persistence = 0.5f;
        float lacunarity = 2.0f;
        
        void DrawControls()
        {
            if (ImGui::SliderInt("##seed", (int *)&seed, 0, std::numeric_limits<int>::max() - 1, "Seed %.0f")) {
                noise.Seed(seed);
            }
            ImGui::SliderInt("##octaves", (int *)&octaves, 1, 10, "Octaves %.0f");
            ImGui::SliderFloat("##frequency", &frequency, 0.0f, 64.0f, "Frequency %.3f");
            ImGui::SliderFloat("##persistence", &persistence, 0.0f, 64.0f, "Persistence %.3f");
            ImGui::SliderFloat("##lacunarity", &lacunarity, 0.0f, 64.0f, "Lacunarity %.3f");
        }

    private:
        noise::PerlinNoise noise;
};

class Voronoi : public Generator
{
    public:
        Voronoi() : Generator("Voronoi"), noise(0) { };

        float Evaluate(float x, float y, float z) const
        {
            return noise.Sample(x, y, z, frequency);
        }

        uint64_t seed = 0;
        float frequency = 1.0f;

        void DrawControls()
        {
            if (ImGui::SliderInt("##seed", (int *)&seed, 0, std::numeric_limits<int>::max() - 1, "Seed %.0f")) {
                noise.Seed(seed);
            }
            ImGui::SliderFloat("##frequency", &frequency, 0.0f, 64.0f, "Frequency %.3f");
        }


    private:
        noise::VoronoiNoise noise;
};

// Base class for filters, nodes that transform one input
class Filter : public Node
{
    public:
        Filter(std::string name) : Node(1, name) { };
};

class Abs : public Filter
{
    public:
        Abs() : Filter("Abs") { };

        void DrawControls() { };

        float Evaluate(float x, float y, float z) const
        {
            const Node *in = InputSlot(0).toNode;

            return  in ? fabs(in->Evaluate(x, y, z)) : 0.0f;
        }
};

class Invert : public Filter
{
    public:
        Invert() : Filter("Invert") { };

        void DrawControls() { };

        float Evaluate(float x, float y, float z) const
        {
            const Node *in = InputSlot(0).toNode;

            return  in ? -in->Evaluate(x, y, z) : 0.0f;
        }
};

// Base class for combiners, nodes that combine two inputs together 
class Combiner : public Node
{
    public:
        Combiner(std::string name) : Node(2, name) { };

        float strength = 1.0;

        void DrawControls()
        {
            ImGui::SliderFloat("##strength", &strength, 0.0f, 1.0f, "Strength %.3f");
        }
};

class Add : public Combiner
{
    public:
        Add() : Combiner("Add") { };

        float Evaluate(float x, float y, float z) const
        {
            const Node *in1 = InputSlot(0).toNode;
            const Node *in2 = InputSlot(1).toNode;

            return  (in1 ? in1->Evaluate(x, y, z) : 0.0f) + (in2 ? in2->Evaluate(x, y, z) * strength : 0.0f);
        }
};