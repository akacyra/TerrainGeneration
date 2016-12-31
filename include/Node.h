#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <string>
#include "PerlinNoise.h"
#include <imgui.h>
#include <cmath>

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

        virtual void DrawControls(ImDrawList *drawList) = 0;

        virtual void Reset() { };
        virtual Node *Clone() = 0;

        ImVec2 pos;
        ImVec2 size;

    protected:
        void InputCount(unsigned count);
        void OutputCount(unsigned count);

    private:
        unsigned inputCount;
        unsigned outputCount; 
        std::vector<Slot> inputSlots;
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
        typedef float (*StyleFunc)(float);

        Perlin() : Generator("Perlin"), noise(0) { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void DrawControls(ImDrawList *drawList);
        void Reset();

        Node *Clone() { return new Perlin(*this); }

        uint64_t seed;
        unsigned octaves;
        float frequency;
        float persistence;
        float lacunarity;

        StyleFunc style;

        static float Classic(float v) { return v; };
        static float Billowy(float v) { return fabs(v - 0.5f) + 0.5f; };
        static float Ridged(float v) { return 0.5f - fabs(v - 0.5f); };

    private:
        noise::PerlinNoise noise;

        int currentStyleIdx;
};

class Constant : public Generator
{
    public:
        Constant() : Generator("Constant") { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void DrawControls(ImDrawList *drawList);
        void Reset();

        Node *Clone() { return new Constant(*this); }
    
        float value;
};

class Gradient : public Generator
{
    public:
        Gradient() : Generator("Gradient") { Reset(); };

        float Evaluate(float x, float y, float z) const;

        void DrawControls(ImDrawList *drawList);
        void Reset();

        Node *Clone() { return new Gradient(*this); }

        ImVec2 start;
        ImVec2 end;
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

        void DrawControls(ImDrawList *drawList) { };

        float Evaluate(float x, float y, float z) const;

        Node *Clone() { return new Abs(*this); }
};

class Invert : public Filter
{
    public:
        Invert() : Filter("Invert") { };

        void DrawControls(ImDrawList *drawList) { };

        float Evaluate(float x, float y, float z) const;

        Node *Clone() { return new Invert(*this); }
};

class Selector : public Filter
{
    public:
        Selector() : Filter("Selector") { Reset(); };

        void Reset();
        void DrawControls(ImDrawList *drawList);

        float Evaluate(float x, float y, float z) const;

        Node *Clone() { return new Selector(*this); }

        float min, max;
        float falloff;
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
        void DrawControls(ImDrawList *drawList);

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
        void DrawControls(ImDrawList *drawList);

        Node *Clone() { return new ImageOutput(*this); };

    private:
        char buffer[128];
        unsigned imageSize;
};


#endif 