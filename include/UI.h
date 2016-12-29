#ifndef __UI_H__
#define __UI_H__

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
void ShowNodeGraphEditor(bool *opened, ApplicationData &data)
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

    // Display links
    drawList->ChannelsSetCurrent(0); // Background
    for (auto pair : data.nodes) {
        Node *node = pair.second;

        Slot outSlot = node->OutputSlot();
        if (outSlot.toNode) {
            ImVec2 p1 = offset + node->OutputSlotPos();
            ImVec2 p2 = offset + outSlot.toNode->InputSlotPos(outSlot.toSlot);
            drawList->AddBezierCurve(p1, p1 + ImVec2(50, 0), p2 + ImVec2(-50, 0), p2, ImColor(200, 200, 200), 3.0f);

        }
    }

    if (data.nodeSelected != -1 && data.slotSelected != -1) {
        Node *node = data.nodes[data.nodeSelected];
        ImVec2 p2 = ImGui::GetMousePos();
        if (data.slotIsOutput) {
             ImVec2 p1 = offset + node->OutputSlotPos();
             drawList->AddBezierCurve(p1, p1 + ImVec2(50, 0), p2 + ImVec2(-50, 0), p2, ImColor(200, 200, 200), 3.0f);
        } else {
             ImVec2 p1 = offset + node->InputSlotPos(data.slotSelected);
             drawList->AddBezierCurve(p1, p1 + ImVec2(-50, 0), p2 + ImVec2(50, 0), p2, ImColor(200, 200, 200), 3.0f);
        }
    }

    bool openContextMenu = false;

    // Display nodes
    for (auto pair : data.nodes) {
        Node *node = pair.second;

        ImGui::PushID(node->ID());
        ImVec2 nodeRectMin = offset + node->pos;

        // Display node contents first
        drawList->ChannelsSetCurrent(1); // Foreground
        bool old_any_active = ImGui::IsAnyItemActive();
        ImGui::SetCursorScreenPos(nodeRectMin + NODE_WINDOW_PADDING);
        ImGui::BeginGroup(); // Lock horizontal position
        ImGui::Text("%s: %p%", node->Name(), node);
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
            data.nodeSelected = node->ID();
            data.slotSelected = -1;
        } if (nodeMovingActive && ImGui::IsMouseDragging(0)) {
            node->pos = node->pos + ImGui::GetIO().MouseDelta;
        }

        ImU32 nodeBGColor  = nodeHoveredInScene == node->ID() ? ImColor(75,75,75) : ImColor(60,60,60);
        drawList->AddRectFilled(nodeRectMin, nodeRectMax, nodeBGColor , 4.0f); 
        drawList->AddRect(nodeRectMin, nodeRectMax, ImColor(100,100,100), 4.0f); 
        for (int slotIdx = 0; slotIdx < node->InputCount(); slotIdx++) {
            ImGui::PushID(slotIdx);

            ImVec2 pos = offset + node->InputSlotPos(slotIdx);
            ImColor color = node->IsInputSlotConnected(slotIdx) ? ImColor(150,150,150,150) : ImColor(150, 100, 100);
            drawList->AddCircleFilled(pos, NODE_SLOT_RADIUS, color);

            if (SlotButton(pos)) {
                if (data.slotSelected == -1) {
                    data.nodeSelected = node->ID();
                    data.slotSelected = slotIdx;
                    data.slotIsOutput = false;
                } else if (data.slotIsOutput) {
                    Node *toNode = data.nodes[data.nodeSelected]; // Ensure data.nodeSelected != -1
                    if (toNode != node) {
                        if (node->IsInputSlotConnected(slotIdx)) {
                            node->DisconnectInputSlot(slotIdx);
                        }

                        if (toNode->IsOutputSlotConnected()) {
                            Slot out = toNode->OutputSlot();
                            out.toNode->DisconnectInputSlot(out.toSlot);
                        }

                        node->ConnectInputSlot(slotIdx, toNode);
                        
                        data.slotSelected = -1;
                    }
                }
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Points to: %p", node->InputSlot(slotIdx).toNode);
            }


            ImGui::PopID();
        }

        ImColor color = node->IsOutputSlotConnected() ? ImColor(150,150,150,150) : ImColor(150, 100, 100);
        drawList->AddCircleFilled(offset + node->OutputSlotPos(), NODE_SLOT_RADIUS, color);

        if (SlotButton(offset + node->OutputSlotPos())) {
            if (data.slotSelected == -1) {
                data.nodeSelected = node->ID();
                data.slotSelected = 0;
                data.slotIsOutput = true;
            } else if (!data.slotIsOutput) {
                Node *fromNode = data.nodes[data.nodeSelected]; // Ensure data.nodeSelected != -1
                if (fromNode != node) {
                    if (fromNode->IsInputSlotConnected(data.slotSelected)) {
                        fromNode->DisconnectInputSlot(data.slotSelected);
                    }
                    fromNode->ConnectInputSlot(data.slotSelected, node);
                    
                    data.slotSelected = -1;
                }
            }
        }

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Points to: %p", node->OutputSlot().toNode);
        }

        ImGui::PopID();
    }

    drawList->ChannelsMerge();

    // Open context menu
    if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1)) {
        if (data.slotSelected == -1) {
            data.nodeSelected = -1;
        }
        nodeHoveredInScene = -1;
        openContextMenu = true;
    }
    if (openContextMenu) {
        ImGui::OpenPopup("context_menu");

        if (nodeHoveredInScene != -1) {
            data.nodeSelected = nodeHoveredInScene;
        }
    }

    // Draw context menu
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8,8));
    if (ImGui::BeginPopup("context_menu")) {
        Node* node = data.nodeSelected != -1 ? data.nodes[data.nodeSelected] : NULL;
        ImVec2 scenePos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
        if (node && data.slotSelected == -1) {
            ImGui::Text("Node '%s'", node->Name());
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", nullptr, false, true)) {
                node->DisconnectAll();
                data.nodes.erase(node->ID());
                delete node;
                data.nodeSelected = -1;
            }
        } else {
            bool connectingToInput = data.slotSelected != -1 && data.slotIsOutput;
            bool connectingToOutput = data.slotSelected != -1 && !data.slotIsOutput;

            Node *newNode = nullptr;
            ImGui::Text("Create Nodes");
            ImGui::Separator();
            if (ImGui::MenuItem("Perlin", nullptr, false, !connectingToInput)) {
                newNode = new Perlin();
            }
            if (ImGui::MenuItem("Voronoi", nullptr, false, !connectingToInput)) {
                newNode = new Voronoi();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Abs", nullptr, false, true)) {
                newNode = new Abs();
            }
            if (ImGui::MenuItem("Invert", nullptr, false, true)) {
                newNode = new Invert();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Add", nullptr, false, true)) {
                newNode = new Add();
            }

            if (newNode) {
                newNode->pos = scenePos;
                data.nodes[newNode->ID()] = newNode;
                if (connectingToInput) {
                    newNode->ConnectInputSlot(0, data.nodes[data.nodeSelected]);
                    data.slotSelected = -1;
                } else if (connectingToOutput) {
                    newNode->ConnectOutputSlot(data.nodes[data.nodeSelected], data.slotSelected);
                    data.slotSelected = -1;
                }
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

    unsigned size = data.previewImageSize;

    ImGui::Image((void *)data.renderTextureID, ImVec2(size, size), ImVec2(0, 0), ImVec2(0.25, 0.25));

    if (data.nodeSelected != -1) {
        Node *node = data.nodes[data.nodeSelected];

        data.renderTextureData.clear();

        for (unsigned i = 0; i < size; i++) {
            for(unsigned j = 0; j < size; j++) {
                float f = node->Evaluate((float)j / size, (float)i / size, 0.0f);
                unsigned char b = (f + 1.0f) / 2.0f * 255;

                data.renderTextureData.push_back(b);
                data.renderTextureData.push_back(b);
                data.renderTextureData.push_back(b);
            }
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, data.renderTextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_INT, data.renderTextureData.data());
    }

    ImGui::End();
}


#endif