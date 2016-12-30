#include "Workspace.h"

Workspace::Workspace()
{ 
    Reset(); 
};

void Workspace::DeleteNode(int id)
{
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        delete it->second;
        nodes.erase(it);

        if (id == selection.Node()) {
            Unselect();
        }
    }
}

Node *Workspace::GetNode(int id) const
{
    auto it = nodes.find(id);
    if (it != nodes.end()) {
        return it->second;
    } 
    return nullptr;
}

Node *Workspace::GetSelectedNode() const
{
    return GetNode(selection.Node());
}

void Workspace::SelectNode(int node)
{
    selection = Selection::SelectNode(node);
}

void Workspace::SelectInputSlot(int node, int slot)
{
    selection = Selection::SelectInput(node, slot);
}

void Workspace::SelectOutputSlot(int node, int slot)
{
    selection = Selection::SelectOutput(node, slot);
}

void Workspace::Unselect()
{
    selection = Selection::SelectNone();
}

const Selection &Workspace::Selection() const
{
    return selection;
}

const Workspace::NodeMap &Workspace::Nodes() const
{
    return nodes;
}

void Workspace::Reset()
{
    Unselect();
    clipboard = nullptr;
    nodes.clear();
}

void Workspace::Copy()
{
    if (selection.HasNode()) {
        clipboard = GetNode(selection.Node())->Clone();
    }
}

void Workspace::Paste(ImVec2 pos)
{
    if (clipboard) {
        Node *node = clipboard;
        node->pos = pos;
        clipboard = nullptr;
        nodes[node->ID()] = node;
    }
}

const Node *Workspace::Clipboard() const
{
    return clipboard;
}