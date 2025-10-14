#pragma once
#include "Node.hpp"
#include "../Editor.hpp"
#include "../ImGuiHelper/Helper.hpp"
#include "../Extension/implot/implot.h"
#include "../Extension/misc/cpp/imgui_stdlib.h"
#include <imgui_internal.h>
//#include "BackendCore/BackEndValue.hpp"
#include <stb_image.h>
#include <iostream>
namespace editor
{
	static const char* label = "Node ID";
	static const char* label_error = "Node ID need Unique";

	Node::Node(const int& id, NodeData data) : 
		m_Id(id),
		m_StrName(data.name),
		m_StrDescription(data.strDescription),
		m_Texture_Button_Excute(data.textureButtonExcute), 
		m_Texture_Node_Icon(data.textureNodeIcon),
		m_Editor(data.editor), 
		m_strId(data.szId), 
		m_Postion(data.postion), 
		m_StrUniqueID(data.strUniqueId)
	{
		if (data.strNodeType == "flow")
		{
			m_nodeType = NodeType::FLow; 
		}
		if (data.strNodeType == "value")
		{
			m_nodeType = NodeType::Value; 
		}
		//if flow node create slot direct 
		if(m_nodeType == NodeType::FLow)
			createSlotDirect(); 
		//Create input with json  
		for (auto& input : data.inputPins)
		{
			m_Inputs.push_back(m_Editor->createSlot({ ax::NodeEditor::PinKind::Input,
				SlotType::Value,
				input["type"].get<std::string>(),
				input["name"].get<std::string>(),
				this,
				m_Editor,
				editor::draw::DrawHelper::createColorFromJson(input["color"])
				})); 
		}
		//Create output with json 
		for (auto& output : data.outputPins)
		{
			m_Outputs.push_back(m_Editor->createSlot({ ax::NodeEditor::PinKind::Output,
				SlotType::Value,
				output["type"].get<std::string>(),
				output["name"].get<std::string>(),
				this,
				m_Editor,
				editor::draw::DrawHelper::createColorFromJson(output["color"])
				}));
		}


		//parse form 

		for (auto& element : data.form)
		{
			//create ui component
			m_ui_component.insert({
				element["key"].get<std::string>(),
				createUIComponent(element) }); 
		}

		//parse value to form 

		for (auto& element : data.formData)
		{
			setValueForComponent(element); 
		}


	}

	Node::~Node()
	{
		m_Editor = nullptr; 
		m_ui_component.clear(); 
	}

	void Node::formOnFrame(ImGuiIO& io, const float& time)
	{
		for (auto& [key, component] : m_ui_component)
		{
			component->OnFrame(io,
				time);
		}
	}

	void Node::endFrame(ImGuiIO& io, const float& time)
	{
		for (auto& [key, component] : m_ui_component)
		{
			component->endFrame(io,
				time);
		}
	}
	void Node::OnFrame(ImGuiIO& io, const float& time)
	{
		if (m_Status == NodeStatus::Running)
		{
			m_CurrentColorNodeBorder = node_border_color::node_running;
		}
		if (m_Status == NodeStatus::Create)
		{
			m_CurrentColorNodeBorder = node_border_color::node_border;
		}
		if (m_Status == NodeStatus::Error)
		{
			m_CurrentColorNodeBorder = node_border_color::node_error;
		}
		if (m_Status == NodeStatus::Completed)
		{
			m_CurrentColorNodeBorder = node_border_color::node_completed;
		}
		ed::PushStyleVar(ed::StyleVar_PinRadius, 5.0f);
		ed::PushStyleColor(ed::StyleColor_NodeBorder, m_CurrentColorNodeBorder);
		ax::NodeEditor::BeginNode(m_Id);
		if (m_Editor->iSFirstFrame())
			ax::NodeEditor::SetNodePosition(m_Id,m_Postion);
		ImGui::PushID(m_Id.AsPointer());
		ImGui::BeginHorizontal("headers");
		ImGui::Image(m_Texture_Node_Icon, ImVec2(24.0f, 24.0f));
		ImGui::Spring(0, 100.0f); 
		ImGui::Text(m_StrName.data());
		m_bIsDoToolTip = ImGui::IsItemHovered() ? true : false;
		ImGui::Spring(0, 150);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0)); 
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.2f)); 
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.3f));
		if (ImGui::ImageButton(m_Texture_Button_Excute,
			ImVec2(24.0f,24.0f)))
		{
			std::cout << "Button click\n"; 
		}
		m_bIsDoToolTipButtonExcute = ImGui::IsItemHovered() ? true : false; 
		ImGui::PopStyleColor(3); 
		ImGui::EndHorizontal(); 
		ImGui::NewLine();


		ImGui::BeginHorizontal(m_Id.AsPointer());
		//create vertical input
		ImGui::BeginVertical("inputs", ImVec2(0, 0));
		for (int i = 0; i < m_Inputs.size(); ++i)
		{
			auto& input = m_Inputs.at(i);
			input->render(io,
				time);
		}
		ImGui::EndVertical();
		if(m_Outputs.size() != 0 && m_Inputs.size() != 0)
			m_DummyVec = ImVec2(m_XDistance - 200, 0.0f); 
		else
			m_DummyVec = ImVec2(m_XDistance - 100, 0.0f);
		ImGui::Dummy(m_DummyVec);
		//create vertical output
		ImGui::BeginVertical("outputs", ImVec2(0, 0));
		for (int i = 0; i < m_Outputs.size(); ++i)
		{
			auto& output = m_Outputs.at(i);
			output->render(io,
				time);
		}
		ImGui::EndVertical();
		ImGui::EndHorizontal();

		m_HeaderMin = ImGui::GetItemRectMin();
		m_HeaderMinSlotSpace = ImGui::GetItemRectMax(); 
		ImGui::PushItemWidth(350);

		ImVec4 text_color{};
		m_bIsNotUniqueId ? text_color = m_ColorTextNotUniqueID : text_color = m_ColorNormalText; 
		ImGui::PushStyleColor(ImGuiCol_Text, text_color);
		m_bIsNotUniqueId ? ImGui::InputTextWithHint(label_error,
			"Enter ID Here",
			&m_StrUniqueID,
			ImGuiInputTextFlags_CallbackAlways,
			InputTextUniqueIDCallback,
			this) : ImGui::InputTextWithHint(label,
				"Enter ID Here",
				&m_StrUniqueID,
				ImGuiInputTextFlags_CallbackAlways,
				InputTextUniqueIDCallback,
				this); 
		ImGui::PopStyleColor(); 
		formOnFrame(io,
			time);
		m_Postion = ax::NodeEditor::GetNodePosition(m_Id); 
		ax::NodeEditor::EndNode();
		ImGui::PopID();
		m_HeaderMax = ImGui::GetItemRectMax();
		//Draw sperator 
		auto drawList = ed::GetNodeBackgroundDrawList(m_Id);
		const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;
		drawList->AddLine(
			ImVec2(m_HeaderMin.x, m_HeaderMin.y - 3.5f),
			ImVec2(m_HeaderMax.x - 8.0f, m_HeaderMin.y),
			ImColor(255, 255, 255, 96 * 255 / (3 * 255)), 3.0f);
		ed::Suspend();
		if (m_bIsDoToolTip)
			ImGui::SetTooltip(m_StrDescription.data());
		if (m_bIsDoToolTipButtonExcute)
			ImGui::SetTooltip(m_StrButtonExcute.c_str()); 
		endFrame(io, time);
		ed::Resume();
		if (m_Editor->iSFirstFrame() && m_bIsFirstFrameNode)
			m_XDistance = m_HeaderMax.x - m_HeaderMin.x;
		m_bIsFirstFrameNode = false; 
		ax::NodeEditor::PopStyleVar();
		ax::NodeEditor::PopStyleColor(); 
	}
	int Node::getIndexSlotInput(ax::NodeEditor::PinId idPin)
	{
		for (int i = 0; i < m_Inputs.size(); ++i)
		{
			auto input = m_Inputs.at(i); 
			if (input->getId() == idPin)
			{
				return i; 
			}
		}
		return -1;
	}
	int Node::getIndexSlotOutput(ax::NodeEditor::PinId idPin)
	{
		for (int i = 0; i < m_Outputs.size(); ++i)
		{
			auto output = m_Outputs.at(i);
			if (output->getId() == idPin)
			{
				return i; 
			}
		}
		return -1;
	}
	ed::PinId Node::getInputPinIdWithIndexPin(const int& index)
	{
		if (index < 0 || index >= m_Inputs.size())
			return ed::PinId(0); 
		return m_Inputs.at(index)->getId(); 
	}
	ed::PinId Node::getOutputPinIdWithIndexPin(const int& index)
	{
		if (index < 0 || index >= m_Outputs.size())
			return ed::PinId(0);
		return m_Outputs.at(index)->getId();
	}
	ImVec2& Node::getPostion()
	{
		return m_Postion; 
	}
	std::string& Node::getStrId()
	{
		return m_strId; 
	}
	//std::string& Node::getStrUniqueID()
	//{
	//	return m_StrUniqueID; 
	//}
	std::vector<Slot*>& Node::getInput()
	{
		return m_Inputs; 
	}
	std::vector<Slot*>& Node::getOutput()
	{
		return m_Outputs; 
	}
	void Node::upateUniqueIdNode()
	{
		m_bIsNotUniqueId = m_Editor->checkIdIsUnique(m_Id.Get(),m_StrUniqueID); 
	}
	bool Node::isNotUniqueID()
	{
		return m_bIsNotUniqueId; 
	}
	std::unique_ptr<UIComponent> Node::createUIComponent(nlohmann::json& component_json)
	{
		std::string type = component_json["type"].get<std::string>(); 
		std::string key = component_json["key"].get<std::string>();
		if (type == "text-area")
		{
			std::string label = component_json["label"].get<std::string>();
			return std::make_unique<TextArea>(key,
				label,
				type);
		}
		if (type == "edit-text")
		{
			std::string label = component_json["label"].get<std::string>();
			std::string hint = component_json["hint"].get<std::string>(); 
			return std::make_unique<EditText>(key,
				label,
				hint,
				type);
		}
		if (type == "input-int")
		{
			std::string label = component_json["label"].get<std::string>();
			return std::make_unique<InputInt>(key,
				label,
				type); 
		}
		if (type == "image-view")
		{
			return std::make_unique<ImageView>(key);
		}
		if (type == "code-editor")
		{
			std::string label = component_json["label"].get<std::string>();
			return std::make_unique<CodeEditor>(key,
				label,
				type);
		}
		return nullptr;
	}
	NodeStatus& Node::getStatus()
	{
		return m_Status; 
	}
	void Node::setValueForComponent(nlohmann::json& element_json)
	{
		if(element_json.is_null()) return; 
		const std::string& key = element_json["key"].get<std::string>();
		if (m_ui_component.count(key) == 0)
		{
			std::cout << "Component not found\n"; 
			return;
		}

		auto component = m_ui_component.at(key).get(); 

		if (component->as<TextArea>())
		{
			component->as<TextArea>()->setText(element_json["value"].get<std::string>()); 
		}

		if (component->as<EditText>())
		{
			component->as<EditText>()->setText(element_json["value"].get<std::string>()); 
		}

		if (component->as<InputInt>())
		{
			component->as<InputInt>()->setNum(element_json["value"].get<int>());
		}
	}
	const std::string& Node::getStrUniqueID()
	{
		return m_StrUniqueID; 
	}
	nlohmann::json Node::serializeFormInput()
	{
		nlohmann::json array_input_form; 
		for (auto& [key, component] : m_ui_component)
		{
			nlohmann::json input; 
			input["key"] = key; 
			if (component->as<TextArea>())
			{
				input["value"] = component->as<TextArea>()->getStr(); 
			}

			if (component->as<EditText>())
			{
				input["value"] = component->as<EditText>()->getStr(); 
			}
			if (component->as<InputInt>())
			{
				input["value"] = component->as<InputInt>()->getNum(); 
			}
			array_input_form.push_back(input); 
		}
		return array_input_form;
	}
	void Node::createSlotDirect()
	{
		m_Inputs.push_back(m_Editor->createSlot({
			ax::NodeEditor::PinKind::Input,
			SlotType::FLow,
			"default",
			"flow",
			this,
			m_Editor
			})); 
		m_Outputs.push_back(m_Editor->createSlot({
			ax::NodeEditor::PinKind::Output,
			SlotType::FLow,
			"default",
			"flow",
			this,
			m_Editor
			})); 
	}
	//void Node::running()
	//{
	//	std::cout << getStrUniqueID() + "::Run" << std::endl; 
	//	m_Status = NodeStatus::Running; 
	//}
	//void Node::complete()
	//{
	//	std::cout << getStrUniqueID() + "::Complete" << std::endl;
	//	m_Status = NodeStatus::Completed;
	//}
	//void Node::error()
	//{
	//}
	//void Node::setInput(std::vector<std::shared_ptr<backend::WFVariant>> lstVariant)
	//{
	//	if (m_strId == "image_view")
	//	{
	//		//create image 
	//		auto& img = std::get<backend::Image>(lstVariant.at(0)->m_variant); 
	//		auto id = m_Editor->getApplication()->CreateTexture(img.data, 
	//			img.m_width, 
	//			img.m_height);
	//		if (id)
	//		{
	//			//remove texture 
	//			auto& old_id = m_ui_component.at("imageview")->as<ImageView>()->getTextureId(); 
	//			m_Editor->getApplication()->DestroyTexture(old_id); 
	//		}
	//		m_ui_component.at("imageview")->as<ImageView>()->setTextureId(id, img.m_width, img.m_height); 
	//		stbi_image_free(img.data);
	//	}

	//	if (m_strId == "code_editor")
	//	{
	//		auto& strText = std::get<std::string>(lstVariant.at(0)->m_variant); 
	//		m_ui_component.at("codeeditor")->as<CodeEditor>()->setText(strText); 
	//	}
	//}
	//void Node::setOutput(std::vector<std::shared_ptr<backend::WFVariant>> lstVariant)
	//{
	//	std::cout << m_strId << std::endl;
	//}
	int Node::InputTextUniqueIDCallback(ImGuiInputTextCallbackData* data)
	{
		Node* node = static_cast<Node*>(data->UserData);
		if(node)node->upateUniqueIdNode();
		return 0; 
	}
}