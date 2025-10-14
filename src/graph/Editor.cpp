#include "Editor.hpp"
# include <imgui_node_editor.h>
# include <imgui_internal.h>
#include <nlohmann/json.hpp>
#include "Extension/implot/implot.h"
//Extension plugin 
#include "PluginFrontend/CreatePrompt.hpp"
#include "NodeApp.hpp"
#include <fstream>
#include <iostream>

namespace editor
{
	Editor::Editor(NodeApplication* application) : m_Application(application)
	{
	}
	Editor::~Editor()
	{
        clear(); 
	}
	void Editor::createEditorContext(const std::string& jsonPath)
	{
        ImPlot::CreateContext(); 
		ed::Config config; 
		config.SettingsFile = jsonPath.c_str();
		m_Context = ed::CreateEditor(&config); 
        //create simple node 
        //loadSchema("");
	}
	void Editor::editorOnFrame(const float& time)
	{
        //editor render 
        auto& io = ImGui::GetIO();
        if (m_bIsShowFPS)
        {
            ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
            ImGui::Separator();
        }
        // Node Editor Widget
        ed::SetCurrentEditor(m_Context);
        ed::Begin(m_strEditorName.c_str(), ImVec2(0.0, 0.0f));
        if (m_bIsFirstFrame)
        {
            auto& editorStyle = ed::GetStyle();
            m_colorBGDark = editorStyle.Colors[0]; 
        }
        switch (m_currentStyle)
        {
        case editor::EditorStyle::DARK:
        {
            auto& editorStyle = ed::GetStyle();
            editorStyle.Colors[0] = m_colorBGDark;
            break;
        }
        case editor::EditorStyle::LIGHT:
        {
            auto& editorStyle = ed::GetStyle();
            editorStyle.Colors[0] = m_colorBGLight;
            break;
        }
        default:
            break;
        }

        for (auto& node : m_Nodes)
        {
            node.second->OnFrame(io,
                time);
        }
        // Link Drawing Section
        //handleLink();
        //show flow 
        //showFlow(); 
        //hover 
        ed::End();
        m_bIsFirstFrame = false; 
        if (m_bIsShowDemoWindow)
        {
            ImGui::ShowDemoWindow();
        }

        if (m_bShowOrdinals)
        {
            drawOrdinals();
        }
        ed::SetCurrentEditor(nullptr);

	}

	void Editor::destroy()
	{
		ed::DestroyEditor(m_Context);
        ImPlot::DestroyContext();
	}

    void Editor::getNextUniqueId(int& pinkId)
    {
        pinkId = m_UniqueId;
        m_UniqueId += 1; 
    }

    NodeApplication* Editor::getApplication()
    {
        return m_Application; 
    }

    bool Editor::iSFirstFrame()
    {
        return m_bIsFirstFrame; 
    }

    Slot* Editor::createSlot(SlotData slotData)
    {
        int id; 
        getNextUniqueId(id); 
        m_mapSlot.insert({
            id,
            std::make_unique<Slot>(id,slotData)
            }); 
        return m_mapSlot.at(id).get();
    }

    Slot* Editor::getSlot(const int& slotId)
    {
        if (m_mapSlot.count(slotId) == 0) return nullptr; 
        return m_mapSlot.at(slotId).get(); 
    }

    void Editor::setStyle(EditorStyle style)
    {
        m_currentStyle = style; 
    }

    nlohmann::json Editor::serializeWorkflowGraph()
    {
        //check if node id not unique 
        nlohmann::json graph_json;
        nlohmann::json nodes_json; 

        for (auto& [id, node] : m_Nodes)
        {
            if (node->isNotUniqueID())
            {
                std::cout << "can't save" << std::endl;
                return graph_json;
            }
        }

        for (auto& [id, node] : m_Nodes)
        {
            auto strId = node->getStrId(); 
            auto pos = node->getPostion();
            nlohmann::json node_json; 
            node_json["position"]["x"] = pos.x; 
            node_json["position"]["y"] = pos.y; 
            node_json["id"] = strId; 
            node_json["uniqueId"] = node->getStrUniqueID(); 
            node_json["form"] = node->serializeFormInput(); 
            nodes_json.push_back(node_json); 
        }
        nlohmann::json links_json; 
        for (auto&[id,link] : m_Links)
        {
            auto inputID = link.sourceId.Get(); 
            auto outputID = link.targetId.Get();
            nlohmann::json link_json; 

            auto sourcePin = getSlot(inputID); 
            auto targetPin = getSlot(outputID); 

            auto nodeSource = sourcePin->getNode();
            auto nodeTarget = targetPin->getNode();

            std::string strSourceUniqueId = nodeSource->getStrUniqueID();
            std::string strTargetUniqueId = nodeTarget->getStrUniqueID();

            int sourcePinIndex = link.indexSourcePin; 
            int targetPinIndex = link.indexTargetPin; 

            link_json["sourceNode"] = strSourceUniqueId; 
            link_json["targetNode"] = strTargetUniqueId; 
            link_json["sourcePinIndex"] = sourcePinIndex; 
            link_json["targetPinIndex"] = targetPinIndex; 
            links_json.push_back(link_json);
        }

        graph_json["nodes"] = nodes_json;
        graph_json["links"] = links_json; 
        return graph_json; 
    }

    bool Editor::deserializeWorkflowGraph(nlohmann::json& graph_json)
    {

        auto& nodes_json = graph_json["nodes"]; 
        auto& links_json = graph_json["links"]; 
        for (auto& node_json : nodes_json)
        {
            std::string node_id = node_json["id"]; 
            std::string uniqueId = node_json["uniqueId"]; 
            ImVec2 node_postion = draw::DrawHelper::createVec2FromJson(node_json["position"]);
            nlohmann::json form_data = node_json["form"]; 
            createNode(node_id, 
                node_postion,
                uniqueId,
                form_data); 
        }
        for (auto& link_json : links_json)
        {
            int sourcePinIndex = link_json["sourcePinIndex"].get<int>();
            int targetPinIndex = link_json["targetPinIndex"].get<int>();
            std::string sourceNodeUniqueId = link_json["sourceNode"]; 
            std::string targetNodeUniqueId = link_json["targetNode"]; 
            auto sourceNode = getNodeWithStrUniqueId(sourceNodeUniqueId); 
            auto targetNode = getNodeWithStrUniqueId(targetNodeUniqueId);

            ed::PinId sourcePinId = sourceNode->getOutputPinIdWithIndexPin(sourcePinIndex); 
            ed::PinId targetPinId = targetNode->getInputPinIdWithIndexPin(targetPinIndex); 

            int id_Link; 
            getNextUniqueId(id_Link); 
            m_Links.insert({
                id_Link,
                { ed::LinkId(id_Link),
                sourcePinId,
                targetPinId,
                sourceNodeUniqueId,
                targetNodeUniqueId,
                sourcePinIndex,
                targetPinIndex }
                }); 

            auto sourceSlot = getSlot(sourcePinId.Get()); 
            auto targetSlot = getSlot(targetPinId.Get()); 

            sourceSlot->enableConnected(id_Link); 
            targetSlot->enableConnected(id_Link); 

        }
        m_bIsFirstFrame = true; 
        return true; 
    }

    void Editor::clear()
    {
        m_Nodes.clear(); 
        m_mapSlot.clear(); 
        m_Links.clear(); 
        m_UniqueId = 1; 
    }

    void Editor::removeLink(const ed::LinkId& idLink)
    {
        auto linkInfo = m_Links.at(idLink.Get());
        auto sourceSlot = getSlot(linkInfo.sourceId.Get());
        auto targetSlot = getSlot(linkInfo.targetId.Get());
        if (sourceSlot) sourceSlot->disableConnected(idLink);
        if (targetSlot) targetSlot->disableConnected(idLink);
        m_Links.erase(idLink.Get());
    }

    bool Editor::checkIdIsUnique(const int& nodeId,
        const std::string& str)
    {
        for (const auto& [id, node] : m_Nodes)
        {
            if (id != nodeId)
            {
                if (str == node->getStrUniqueID())
                    return true;
            }
        }
        return false; 
    }

    nlohmann::json Editor::getJsonSchema()
    {
        return m_jsonSchema; 
    }

    std::map<int, std::unique_ptr<Node>>& Editor::getNodes()
    {
        return m_Nodes; 
    }

    void Editor::setStatus(EditorStatus status)
    {
        m_status = status; 
    }

    EditorStatus& Editor::getStatus()
    {
        return m_status; 
    }

    void Editor::deleteNode(ed::NodeId& nodeId)
    {
        int id = nodeId.Get();
        if (m_Nodes.count(id) == 0)
        {
            std::cout << "Node not exist" << std::endl; 
            return; 
        }

        auto& node = m_Nodes.at(id);
        auto node_inputs = node->getInput(); 
        auto node_outputs = node->getOutput();

        for (auto& input : node_inputs)
        {
            deleteSlot(input->getId()); 
        }

        for (auto& output : node_outputs)
        {
            deleteSlot(output->getId()); 
        }

        m_Nodes.erase(id); 
        //m_UniqueId--; 
        std::cout << "UniqueID: " << m_UniqueId << std::endl;
    }

    void Editor::deleteSlot(ed::PinId& pinId)
    {
        int id = pinId.Get(); 
        if (m_mapSlot.count(id) == 0)
        {
            std::cout << "Slot not exist" << std::endl; 
            return; 
        }
        m_mapSlot.erase(id); 
        //m_UniqueId--; 
    }

    void Editor::loadSchema(const std::string& path)
    {
        std::string schema_path = "Schema//NodeSchema.json"; 
        std::ifstream ifs(schema_path);
        m_jsonSchema = nlohmann::json::parse(ifs);
        
        //load icon node 
        std::string node_icon_path = ".//RES//editor//icon//default_plugin_icon.png";
        m_Application->LoadTexture(node_icon_path.c_str()
            ,"default_node_icon");
    }

    Node* Editor::createNode(const std::string& idNodeSchema, 
        const ImVec2& postion,
        const std::string& strUniqueId, 
        const nlohmann::json& formData)
    {
        auto& nodes_schema = m_jsonSchema["nodes"]; 
        for (auto& node_schema : nodes_schema)
        {
            if (node_schema["id"] == idNodeSchema)
            {
                std::string node_name = node_schema["name"]; 
                std::string node_type = node_schema["type"]; 
                std::string node_description = node_schema["description"]; 
                auto inputPins_schema = node_schema["inputPins"]; 
                auto outputPins_schema = node_schema["outputPins"]; 
                auto form_schema = node_schema["form"]; 
                int node_id; 
                auto texture_button_excute = m_Application->GetTextureIDWithName("icon_node_button_excute");
                auto texture_node_icon = m_Application->GetTextureIDWithName("default_node_icon");
                getNextUniqueId(node_id);
                m_Nodes.insert({
                    node_id,
                    std::make_unique<Node>(node_id,
                        NodeData(idNodeSchema,
                            node_name,
                            node_type,
                            strUniqueId,
                            node_description,
                            texture_button_excute,
                            texture_node_icon,
                            postion,
                            this,
                            inputPins_schema,
                            outputPins_schema,
                            form_schema, 
                            formData))
                    });
                break; 
            }
        }
        m_bIsFirstFrame = true; 
        return nullptr; 
    }

    Node* Editor::getNode(ed::NodeId id)
    {
        int iId = id.Get(); 
        if (m_Nodes.count(iId) == 0)
            return nullptr;
        return m_Nodes.at(iId).get(); 
    }

    Node* Editor::getNodeWithStrUniqueId(const std::string& uniqueId)
    {
        for (auto& [id, node] : m_Nodes)
        {
            if (node->getStrUniqueID() == uniqueId)
                return node.get(); 
        }
        return nullptr;
    }

    void Editor::handleLink()
    {
        //draw link 
        for (auto& [id,linkInfo] : m_Links)
        {
            //get color link 
            ed::PinId input_id = linkInfo.sourceId; 
            auto slot = getSlot(input_id.Get()); 
            ed::Link(linkInfo.Id, 
                linkInfo.sourceId,
                linkInfo.targetId,
                slot->getColor(),
                m_linkThickness);
        }
        //handle create link 
        //handleHover();

        if (m_status != EditorStatus::Running)
        {
            if (ed::BeginCreate())
            {
                ed::PinId inputPinId, outputPinId;
                if (ed::QueryNewLink(&inputPinId, &outputPinId))
                {
                    if (inputPinId && outputPinId)
                    {
                        auto slotFirst = getSlot(inputPinId.Get());
                        auto slotSecond = getSlot(outputPinId.Get());
                        if (inputPinId == outputPinId)
                        {
                            ed::RejectNewItem(m_colorRejectLink, 2.0f);
                            showLabel(m_strRejectNewLink, m_colorLabel);
                        }
                        else
                        {
                            if (slotFirst->getStrSlotType() != slotSecond->getStrSlotType())
                            {
                                ed::RejectNewItem(m_colorRejectLink, 2.0f);
                                showLabel(m_strRejectNewLink, m_colorLabel);
                            }
                            else
                            {
                                if (slotFirst->getNode() == slotSecond->getNode())
                                {
                                    ed::RejectNewItem(m_colorRejectLink, 2.0f);
                                    showLabel(m_strRejectNewLink, m_colorLabel);
                                }
                                else
                                {

                                    if (inputPinId != outputPinId
                                        && slotFirst->getStrSlotType() == slotSecond->getStrSlotType())
                                    {
                                        if (slotFirst->getPinKind() == slotSecond->getPinKind())
                                        {
                                            ed::RejectNewItem(m_colorRejectLink, 2.0f);
                                            showLabel(m_strRejectNewLink, m_colorLabel);
                                        }
                                        else
                                        {
                                            //check connector is exist 
                                            bool bIsLinkNotExist{ true };
                                            auto linkIds = slotSecond->getConnectedLinkId();
                                            if (linkIds.size() != 0)
                                            {
                                                auto linkId = linkIds[0];
                                                if (m_Links.count(linkId.Get()))
                                                {
                                                    auto link_info = m_Links.at(linkId.Get());
                                                    if (link_info.sourceId == slotFirst->getId())
                                                    {
                                                        ed::RejectNewItem(m_colorRejectLink, 2.0f);
                                                        showLabel(m_strRejectNewLink, m_colorLabel);
                                                        bIsLinkNotExist = false;
                                                    }

                                                }
                                            }
                                            if (bIsLinkNotExist)
                                            {
                                                showLabel(m_strCreateNewLink, m_colorLabel);
                                                if (ed::AcceptNewItem(m_colorAccecptLink, m_linkThickness))
                                                {
                                                    if (slotFirst->getPinKind() == ed::PinKind::Output
                                                        && slotSecond->getPinKind() == ed::PinKind::Input)
                                                    {
                                                        //get Node  
                                                        auto nodeSource = slotFirst->getNode();
                                                        auto nodeTarget = slotSecond->getNode();
                                                        int indexSourcePin = nodeSource->getIndexSlotOutput(inputPinId);
                                                        int indexTargetPin = nodeTarget->getIndexSlotInput(outputPinId);
                                                        std::string strSourceUniqueId = nodeSource->getStrUniqueID();
                                                        std::string strTargetUniqueId = nodeTarget->getStrUniqueID();
                                                        auto linkIdConnectdInput = slotSecond->getConnectedLinkId();

                                                        if (linkIdConnectdInput.size() != 0 && !slotSecond->isCanMultiConnected())
                                                        {
                                                            //remove link 
                                                            removeLink(linkIdConnectdInput[0]);
                                                        }

                                                        int id_Link;
                                                        getNextUniqueId(id_Link);
                                                        m_Links.insert({
                                                            id_Link,
                                                            { ed::LinkId(id_Link),
                                                            inputPinId,
                                                            outputPinId,
                                                            strSourceUniqueId,
                                                            strTargetUniqueId,
                                                            indexSourcePin,
                                                            indexTargetPin }
                                                            });
                                                        slotFirst->enableConnected(id_Link);
                                                        slotSecond->enableConnected(id_Link);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            ed::EndCreate();
            //handle delete link 
            if (ed::BeginDelete())
            {
                ed::NodeId deletedNodeId;
                while (ed::QueryDeletedNode(&deletedNodeId))
                {
                    if (ed::AcceptDeletedItem(&deletedNodeId))
                    {
                        //deleted node
                        deleteNode(deletedNodeId);
                    }
                }
                ed::LinkId deletedLinkId;
                while (ed::QueryDeletedLink(&deletedLinkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        removeLink(deletedLinkId);
                    }
                }
            }
            ed::EndDelete();
        }
    }
    void Editor::showLabel(const std::string& label,
        const ImColor& color)
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
        auto size = ImGui::CalcTextSize(label.c_str());

        auto padding = ImGui::GetStyle().FramePadding;
        auto spacing = ImGui::GetStyle().ItemSpacing;

        ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

        auto rectMin = ImGui::GetCursorScreenPos() - padding;
        auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

        auto drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
        ImGui::TextUnformatted(label.c_str());
    }

    void Editor::showFlow()
    {
       //show flow 
       for (auto& [id, linkInfo] : m_Links)
       {
           //get color link

           const std::string& source_unique_id = linkInfo.m_strUniqueIdSource; 
           auto node = getNodeWithStrUniqueId(source_unique_id); 
           if (node->getStatus() == NodeStatus::Running)
           {
               ed::Flow(id);
           }
       }
    }

    void Editor::handleHover()
    {
        auto pinId = ed::GetHoveredPin(); 
        if (pinId)
        {
            auto slot = getSlot(pinId.Get());
            auto strSlotType = slot->getStrSlotType(); 
            showLabel(strSlotType, m_colorLabel); 
        }
    }
    void Editor::drawOrdinals()
    {
        auto editorMin = ImGui::GetItemRectMin();
        auto editorMax = ImGui::GetItemRectMax();
        int ordinal = 1;
        auto drawList = ImGui::GetWindowDrawList();
        drawList->PushClipRect(editorMin, editorMax);
        for (auto& [nodeId,node] : m_Nodes)
        {
            auto p0 = ed::GetNodePosition(nodeId);
            auto p1 = p0 + ed::GetNodeSize(nodeId);
            p0 = ed::CanvasToScreen(p0);
            p1 = ed::CanvasToScreen(p1);
            ImGuiTextBuffer builder;
            builder.appendf("#%d", ordinal++);
            auto textSize = ImGui::CalcTextSize(builder.c_str());
            auto padding = ImVec2(2.0f, 2.0f);
            auto widgetSize = textSize + padding * 2;
            auto widgetPosition = ImVec2(p1.x, p0.y) + ImVec2(0.0f, -widgetSize.y);
            drawList->AddRectFilled(widgetPosition, widgetPosition + widgetSize, IM_COL32(100, 80, 80, 190), 3.0f, ImDrawFlags_RoundCornersAll);
            drawList->AddRect(widgetPosition, widgetPosition + widgetSize, IM_COL32(200, 160, 160, 190), 3.0f, ImDrawFlags_RoundCornersAll);
            drawList->AddText(widgetPosition + padding, IM_COL32(255, 255, 255, 255), builder.c_str());
        }

        drawList->PopClipRect();
    }
}
