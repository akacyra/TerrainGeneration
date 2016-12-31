#ifndef __WORKSPACE_H__
#define __WORKSPACE_H__

#include "Node.h"
#include <unordered_map>

class Selection
{
    public:
        static Selection SelectNone() { return Selection(-1, -1, -1); };
        static Selection SelectNode(int node) { return Selection(node, -1, -1); };
        static Selection SelectInput(int node, int slot) { return Selection(node, slot, -1); };
        static Selection SelectOutput(int node, int slot) { return Selection(node, -1, slot); };

        Selection() : Selection(-1, -1, -1) { };

        int Node() const { return node; }
        int InputSlot() const { return inputSlot; };
        int OutputSlot() const { return outputSlot; };

        bool HasNode() const { return node != -1; };
        bool HasSlot() const { return HasInputSlot() || HasOutputSlot(); };
        bool HasInputSlot() const { return inputSlot != -1; };
        bool HasOutputSlot() const { return outputSlot != -1; };

    private:
        Selection(int node, int inputSlot, int outputSlot) : node(node), inputSlot(inputSlot), outputSlot(outputSlot) { };

        int node;
        int inputSlot;
        int outputSlot;
};

class Workspace
{
    public:

        typedef std::unordered_map<int, Node *> NodeMap;

        Workspace();

        template<class DerivedNode>
        Node *CreateNode(ImVec2 pos = ImVec2(0, 0))
        {
            DerivedNode *node = new DerivedNode();
            node->pos = pos;
            nodes[node->ID()] = node;
            return node;
        }

        void DeleteNode(int id);

        Node *GetNode(int id) const;
        Node *GetSelectedNode() const;

        void SelectNode(int node);
        void SelectInputSlot(int node, int slot);
        void SelectOutputSlot(int node, int slot);
        void Unselect();
        const Selection &Selection() const;

        const NodeMap &Nodes() const;
        
        void Reset();

        void Copy();
        void Paste(ImVec2 pos = ImVec2(0, 0));
        const Node *Clipboard() const;

        void LockPreviewNode(int id);
        void UnlockPreviewNode();
        bool HasPreviewNode();
        const Node *PreviewNode() const;

    private:
        NodeMap nodes;
        class Selection selection;
        Node *clipboard;
        int previewNode;
};

#endif