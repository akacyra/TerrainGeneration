#include "UI.h"

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

const float NODE_SLOT_RADIUS = 4.0f;
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

bool SlotButton(ImVec2 pos)
{
    ImVec2 oldPos = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(pos - ImVec2(NODE_SLOT_RADIUS * 2, NODE_SLOT_RADIUS * 2));
    bool result = ImGui::InvisibleButton("slot", ImVec2(NODE_SLOT_RADIUS * 4, NODE_SLOT_RADIUS * 4));
    ImGui::SetCursorScreenPos(oldPos);

    return result;
}

// Adapted from the node graph example by Ocornut: https://gist.github.com/ocornut/7e9b3ec566a333d725d4
void ShowNodeGraphEditor(bool *opened, Workspace &workspace, NodeRenderer &renderer, GLuint previewTextureID)
{
    ImGui::SetNextWindowSize(ImVec2(800,600), ImGuiSetCond_FirstUseEver);
    if (!ImGui::Begin("Node Graph Editor", opened, ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        ImGui::End();
        return;
    }

    int nodeHoveredInScene = -1;

    ImGui::SameLine();
    ImGui::BeginGroup();

    // Create our child canvas
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1,1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(60,60,70,200));
    ImGui::BeginChild("scrolling_region", ImVec2(0,0), true, ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoMove);
    ImGui::PushItemWidth(120.0f);

    ImVec2 offset = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSplit(2);

    const Workspace::NodeMap &nodes = workspace.Nodes(); 

    // Display links
    drawList->ChannelsSetCurrent(0); // Background
    for (auto pair : nodes) {
        Node *node = pair.second;

        for (unsigned i = 0; i < node->OutputCount(); i++) {
            Slot outSlot = node->OutputSlot(i);
            if (outSlot.toNode) {
                ImVec2 p1 = offset + node->OutputSlotPos(i);
                ImVec2 p2 = offset + outSlot.toNode->InputSlotPos(outSlot.toSlot);
                drawList->AddBezierCurve(p1, p1 + ImVec2(50, 0), p2 + ImVec2(-50, 0), p2, ImColor(200, 200, 200), 3.0f);
            }
        }
    }

    const Selection &selection = workspace.Selection();

    if (selection.HasSlot()) {
        Node *node = workspace.GetSelectedNode();
        ImVec2 p2 = ImGui::GetMousePos();
        if (selection.HasOutputSlot()) {
             ImVec2 p1 = offset + node->OutputSlotPos(selection.OutputSlot());
             drawList->AddBezierCurve(p1, p1 + ImVec2(50, 0), p2 + ImVec2(-50, 0), p2, ImColor(200, 200, 200), 3.0f);
        } else {
             ImVec2 p1 = offset + node->InputSlotPos(selection.InputSlot());
             drawList->AddBezierCurve(p1, p1 + ImVec2(-50, 0), p2 + ImVec2(50, 0), p2, ImColor(200, 200, 200), 3.0f);
        }
    }

    bool openContextMenu = false;

    // Display nodes
    for (auto pair : nodes) {
        Node *node = pair.second;

        ImGui::PushID(node->ID());
        ImVec2 nodeRectMin = offset + node->pos;

        // Display node contents first
        drawList->ChannelsSetCurrent(1); // Foreground
        bool old_any_active = ImGui::IsAnyItemActive();
        ImGui::SetCursorScreenPos(nodeRectMin + NODE_WINDOW_PADDING);
        ImGui::BeginGroup(); // Lock horizontal position
        //ImGui::Text("%s: %d: %p%", node->Name(), node->ID(), node);
        ImGui::Text("%s", node->Name());
        node->DrawControls();
        ImGui::EndGroup();

        // Save the size of what we have emitted and whether any of the widgets are being used
        bool nodeWidgetsActive = (!old_any_active && ImGui::IsAnyItemActive());
        node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
        ImVec2 nodeRectMax = nodeRectMin + node->size;

        // Display node box
        drawList->ChannelsSetCurrent(0); // Background
        ImGui::SetCursorScreenPos(nodeRectMin);
        ImGui::InvisibleButton("node", node->size);

        if (ImGui::IsItemHovered())
        {
            nodeHoveredInScene = node->ID();
            openContextMenu |= ImGui::IsMouseClicked(1);
        }
        bool nodeMovingActive = ImGui::IsItemActive();
        if (nodeWidgetsActive || nodeMovingActive) {
            workspace.SelectNode(node->ID());
        } if (nodeMovingActive && ImGui::IsMouseDragging(0)) {
            node->pos = node->pos + ImGui::GetIO().MouseDelta;
        }

        ImU32 nodeBGColor  = nodeHoveredInScene == node->ID() ? ImColor(75,75,75) : ImColor(60,60,60);
        drawList->AddRectFilled(nodeRectMin, nodeRectMax, nodeBGColor , 4.0f); 
        drawList->AddRect(nodeRectMin, nodeRectMax, ImColor(100,100,100), 4.0f); 

        for (int slotIdx = 0; slotIdx < node->InputCount(); slotIdx++) {
            ImGui::PushID(slotIdx);

            ImVec2 pos = offset + node->InputSlotPos(slotIdx) - ImVec2(NODE_SLOT_RADIUS / 2, 0);
            ImColor color = node->IsInputSlotConnected(slotIdx) ? ImColor(150,150,150,150) : ImColor(150, 100, 100);
            drawList->AddCircleFilled(pos, NODE_SLOT_RADIUS, color);

            if (SlotButton(pos)) {
                if (!selection.HasSlot()) {
                    workspace.SelectInputSlot(node->ID(), slotIdx);
                } else if (selection.HasOutputSlot()) {
                    Node *toNode = workspace.GetSelectedNode();
                    if (toNode != node) {
                        if (node->IsInputSlotConnected(slotIdx)) {
                            node->DisconnectInputSlot(slotIdx);
                        }

                        if (toNode->IsOutputSlotConnected(selection.OutputSlot())) {
                            Slot out = toNode->OutputSlot(selection.OutputSlot());
                            out.toNode->DisconnectInputSlot(out.toSlot);
                        }

                        node->ConnectInputSlot(slotIdx, toNode, selection.OutputSlot());
                        
                        workspace.Unselect();
                    }
                }
            }

            if (ImGui::IsItemHovered()) {
                //ImGui::SetTooltip("Points to: %p", node->InputSlot(slotIdx).toNode);
            }


            ImGui::PopID();
        }

        for (int slotIdx = 0; slotIdx < node->OutputCount(); slotIdx++) {
            ImGui::PushID(node->InputCount() + slotIdx);

            ImVec2 pos = offset + node->OutputSlotPos(slotIdx) + ImVec2(NODE_SLOT_RADIUS / 2, 0);
            ImColor color = node->IsOutputSlotConnected(slotIdx) ? ImColor(150,150,150,150) : ImColor(150, 100, 100);
            drawList->AddCircleFilled(pos, NODE_SLOT_RADIUS, color);

            if (SlotButton(pos)) {
                if (!selection.HasSlot()) {
                    workspace.SelectOutputSlot(node->ID(), slotIdx);
                } else if (selection.HasInputSlot()) {
                    Node *fromNode = workspace.GetSelectedNode();
                    if (fromNode != node) {
                        if (fromNode->IsInputSlotConnected(selection.InputSlot())) {
                            fromNode->DisconnectInputSlot(selection.InputSlot());
                        }
                        fromNode->ConnectInputSlot(selection.InputSlot(), node, slotIdx);
                        
                        workspace.Unselect();
                    }
                }
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Points to: %p", node->OutputSlot(slotIdx).toNode);
            }

            ImGui::PopID();
        }

        ImGui::PopID();
    }

    drawList->ChannelsMerge();

    // Open context menu
    if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1)) {
        if (!selection.HasSlot()) {
            workspace.Unselect();
        }
        nodeHoveredInScene = -1;
        openContextMenu = true;
    }
    if (openContextMenu) {
        ImGui::OpenPopup("context_menu");

        if (nodeHoveredInScene != -1) {
            workspace.SelectNode(nodeHoveredInScene);
        }
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));
    if (ImGui::BeginPopup("context_menu")) {
        Node* node = workspace.GetSelectedNode();
        ImVec2 scenePos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
        if (node && !selection.HasSlot()) {
            ImGui::Text("Node '%s'", node->Name());
            ImGui::Separator();
            if (ImGui::MenuItem("Reset", nullptr, false, true)) {
                node->Reset();
            }
            if (ImGui::MenuItem("Delete", nullptr, false, true)) {
                node->DisconnectAll();
                workspace.DeleteNode(node->ID());
            }
            if (ImGui::MenuItem("Copy", nullptr, false, true)) {
                workspace.Copy();
            }
        } else {
            bool connectingToInput = selection.HasSlot() && selection.HasOutputSlot();
            bool connectingToOutput = selection.HasSlot() && selection.HasInputSlot();
            bool canPaste = workspace.Clipboard() != nullptr;

            Node *newNode = nullptr;
            ImGui::Text("Create Nodes");
            ImGui::Separator();
            if (ImGui::MenuItem("Perlin", nullptr, false, !connectingToInput)) {
                newNode = workspace.CreateNode<Perlin>(scenePos);
            }
            if (ImGui::MenuItem("Voronoi", nullptr, false, !connectingToInput)) {
                newNode = workspace.CreateNode<Voronoi>(scenePos);
            }
            if (ImGui::MenuItem("Constant", nullptr, false, !connectingToInput)) {
                newNode = workspace.CreateNode<Constant>(scenePos);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Abs", nullptr, false, true)) {
                newNode = workspace.CreateNode<Abs>(scenePos);
            }
            if (ImGui::MenuItem("Invert", nullptr, false, true)) {
                newNode = workspace.CreateNode<Invert>(scenePos);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Combine", nullptr, false, true)) {
                newNode = workspace.CreateNode<Combine>(scenePos);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Image Output", nullptr, false, true)) {
                newNode = workspace.CreateNode<ImageOutput>(scenePos);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Splitter", nullptr, false, true)) {
                newNode = workspace.CreateNode<Splitter>(scenePos);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Paste", nullptr, false, canPaste)) {
                workspace.Paste(scenePos);
            }

            if (newNode) {
                if (connectingToInput) {
                    newNode->ConnectInputSlot(0, workspace.GetNode(selection.Node()), selection.OutputSlot());
                } else if (connectingToOutput) {
                    newNode->ConnectOutputSlot(0, workspace.GetNode(selection.Node()), selection.InputSlot());
                }
                workspace.Unselect();
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
    ImGui::EndGroup();

    ImGui::End();

    

    ImGui::Begin("Preview", nullptr, ImGuiWindowFlags_NoResize);

    unsigned size = renderer.ImageSize();

    ImGui::Image((ImTextureID)previewTextureID, ImVec2(size, size), ImVec2(0, 0), ImVec2(0.25, 0.25));

    if (selection.HasNode()) {
        Node *node = workspace.GetSelectedNode();

        const NodeRenderer::ImageData &image = renderer.Render(node);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, previewTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_INT, image.data());
    }

    ImGui::End();
}