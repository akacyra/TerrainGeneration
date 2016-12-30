#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <string>
#include "PerlinNoise.h"
#include "VoronoiNoise.h"
#include <imgui.h>

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

        Node(unsigned inputCount, unsigned outputCount, std::string name);
        Node(const Node &other);

        virtual ~Node() { };

        unsigned InputCount() const;
        unsigned OutputCount() const;
        const char *Name() const;
        int ID() const;

        bool IsInputSlotConnected(unsigned slotNum) const;
        bool IsOutputSlotConnected(unsigned slotNum) const;
        void ConnectInputSlot(unsigned thisSlot, Node *toNode, unsigned toSlot);
        void DisconnectInputSlot(unsigned slotNum);
        void ConnectOutputSlot(unsigned thisSlot, Node *toNode, unsigned toSlot);
        void DisconnectOutputSlot(unsigned slotNum);
        void DisconnectAll();
        Slot InputSlot(unsigned slotNum) const;
        Slot OutputSlot(unsigned slotNum) const;
        ImVec2 InputSlotPos(unsigned slotNum) const;
        ImVec2 OutputSlotPos(unsigned slotNum) const;

        virtual float Evaluate(float x, float y, float z) const = 0;

        virtual void DrawControls() = 0;

        virtual void Reset() { };
        virtual Node *Clone() = 0;

        ImVec2 pos;
        ImVec2 size;

    protected:
        void InputCount(unsigned count);
        void OutputCount(unsigned count);

    private:
        unsigned inputCount;
        std::vector<Slot> inputSlots;

        unsigned outputCount; 
        std::vector<Slot> outputSlots;

        std::string name;

        static int idCounter;
        int id;
};



// Base class for generators, nodes that generate noise values
class Generator : public Node
{
    public:
        Generator(std::string name) : Node(0, 1, name) { };
};

class Perlin : public Generator
{
    public:
        Perlin() : Generator("Perlin"), noise(0) { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void DrawControls();
        void Reset();

        Node *Clone() { return new Perlin(*this); }

        uint64_t seed;
        unsigned octaves;
        float frequency;
        float persistence;
        float lacunarity;

    private:
        noise::PerlinNoise noise;
};

class Voronoi : public Generator
{
    public:
        Voronoi() : Generator("Voronoi"), noise(0) { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void DrawControls();
        void Reset();

        Node *Clone() { return new Voronoi(*this); }

        uint64_t seed;
        float frequency;

    private:
        noise::VoronoiNoise noise;
};

class Constant : public Generator
{
    public:
        Constant() : Generator("Constant") { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void DrawControls();
        void Reset();

        Node *Clone() { return new Constant(*this); }
    
        float value;
};

// Base class for filters, nodes that transform one input
class Filter : public Node
{
    public:
        Filter(std::string name) : Node(1, 1, name) { };
};

class Abs : public Filter
{
    public:
        Abs() : Filter("Abs") { };

        void DrawControls() { };

        float Evaluate(float x, float y, float z) const;

        Node *Clone() { return new Abs(*this); }
};

class Invert : public Filter
{
    public:
        Invert() : Filter("Invert") { };

        void DrawControls() { };

        float Evaluate(float x, float y, float z) const;

        Node *Clone() { return new Invert(*this); }
};

// Base class for combiners, nodes that combine two inputs together 
class Combiner : public Node
{
    public:
        Combiner(std::string name) : Node(2, 1, name) { };

};

class Combine : public Combiner
{
    public:
        typedef float (*CombineFunc)(float, float);

        Combine() : Combiner("Combine") { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void Reset();
        void DrawControls();

        Node *Clone() { return new Combine(*this); };

        float strength;
        CombineFunc func;

        static float Add(float a, float b) { return a + b; };
        static float Multiply(float a, float b) { return a * b; };

    private:
        int currentFuncIdx;
};

// Base class for output nodes
class Output : public Node
{
    public:
        Output(std::string name) : Node(1, 0, name) { };
};

class ImageOutput : public Output
{
    public:
        ImageOutput() : Output("Image Output") { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void Reset();
        void DrawControls();

        Node *Clone() { return new ImageOutput(*this); };

    private:
        char buffer[128];
        unsigned imageSize;
};

// Utility node that splits one input into n outputs
class Splitter : public Node
{
    public:
        Splitter() : Node(1, 2, "Splitter") { };

        float Evaluate(float x, float y, float z) const;

        void Reset() { };
        void DrawControls();

        Node *Clone() { return new Splitter(*this); };

    private:
        int count = 2;
};

#endif 