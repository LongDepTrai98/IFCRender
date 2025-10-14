//#include "stdafx.h"
#include "Slot.hpp"
//#include "../Editor.hpp"
#include "Node.hpp"
#include "../Editor.hpp"


namespace editor
{
	Slot::Slot(int id,
		SlotData data) : m_Id(id),
		m_PinKind(data.pinkind),
		m_SlotType(data.slotType),
		m_strSlotType(data.strSlotType),
		m_Editor(data.editor),
		m_Node(data.node), 
		m_Color(data.color), 
		m_strName(data.strSlotName)
	{
		std::string slot_name{};
		if (m_PinKind == ax::NodeEditor::PinKind::Input)
		{
			if (m_SlotType == SlotType::FLow)
			{
				m_strName = m_strNameFlowEnter; 
				m_Color = ImColor(1.0f, 1.0f, 1.0f, 1.0f); 
				m_bIsMultiConnected = true; 
			}
		}
		if (m_PinKind == ax::NodeEditor::PinKind::Output)
		{
			if (m_SlotType == SlotType::FLow)
			{
				m_strName = m_strNameFlowOut; 
				m_Color = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
				m_bIsMultiConnected = true;
			}
		}
		if (m_SlotType == SlotType::Value)
		{
			m_IconType = IconType::Circle;
		}
	}

	Slot::~Slot()
	{
		m_Editor = nullptr; 
		m_Node = nullptr; 
	}

	void Slot::render(ImGuiIO& io, const float& deltaTime)
	{
		//render slot 
		//ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
		//ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
		ed::PushStyleVar(ed::StyleVar_PinCorners, ImDrawFlags_RoundCornersBottom);
		ImGui::BeginHorizontal(m_Id.AsPointer());
		if (m_PinKind == ed::PinKind::Output)
			ImGui::Text(m_strName.c_str()); 
		ed::BeginPin(m_Id, m_PinKind);
		if (m_PinKind == ed::PinKind::Input)
		{
			//icon truoc text sau 
			DrawPinIcon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)));
		}
		if (m_PinKind == ed::PinKind::Output)
		{
			//text truoc icon sau 
			DrawPinIcon(ImVec2(static_cast<float>(m_PinIconSize), static_cast<float>(m_PinIconSize)));
		}
		//Draw icon pin 
		ed::EndPin(); 
		if(m_PinKind == ed::PinKind::Input)
			ImGui::Text(m_strName.c_str());
		ImGui::EndHorizontal();
		ed::PopStyleVar(1); 
	}

	ax::NodeEditor::PinId& Slot::getId()
	{
		return m_Id; 
	}
	ax::NodeEditor::PinKind& Slot::getPinKind()
	{
		return m_PinKind; 
	}
	SlotType& Slot::getSlotType()
	{
		return m_SlotType; 
	}
	std::string& Slot::getStrSlotType()
	{
		return m_strSlotType; 
	}
	Node* Slot::getNode()
	{
		return m_Node; 
	}
	ImColor& Slot::getColor()
	{
		return m_Color; 
	}
	void Slot::enableConnected(ax::NodeEditor::LinkId linkId)
	{
		m_bIsConnected = true; 
		setLinkId(linkId); 
	}
	bool Slot::isCanMultiConnected()
	{
		return m_bIsMultiConnected; 
	}
	void Slot::setLinkId(ax::NodeEditor::LinkId linkId)
	{
		//m_LinkId = linkId; 
		m_LinkIds.push_back(linkId); 
		int a = 3; 
	}
	std::vector<ax::NodeEditor::LinkId>& Slot::getConnectedLinkId()
	{
		return m_LinkIds; 
	}
	void Slot::disableConnected(ax::NodeEditor::LinkId link_remove)
	{
		auto it = std::find(m_LinkIds.begin(), m_LinkIds.end(), link_remove); 
		if (it != m_LinkIds.end())
		{
			m_LinkIds.erase(it); 
		}
		if (m_LinkIds.size() == 0)
		{
			m_bIsConnected = false; 
		}
		//setLinkId(0); 
	}
	void Slot::DrawPinIcon(const ImVec2& size)
	{
		if (ImGui::IsRectVisible(size))
		{
			//draw circle 
			auto cursorPos = ImGui::GetCursorScreenPos();
			auto drawList = ImGui::GetWindowDrawList();

			ImColor innerColor; 

			!m_bIsConnected ? innerColor = m_InnerColor : innerColor = m_Color; 

			editor::draw::DrawHelper::DrawIcon(drawList,
				cursorPos,
				ImVec2(cursorPos + size),
				m_IconType,
				false,
				m_Color,
				innerColor);

		}
		ImGui::Dummy(size);
	}
}