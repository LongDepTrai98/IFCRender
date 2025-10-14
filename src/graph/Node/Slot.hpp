#pragma once
#ifndef _SLOT_HPP_
#define _SLOT_HPP_
#include <imgui_node_editor.h>
#include "../ImGuiHelper/Helper.hpp"
namespace editor
{
	class Editor;
	class Node;
	enum class SlotType : ImU32 { FLow,Value };
	struct SlotData
	{
		ax::NodeEditor::PinKind pinkind;
		SlotType slotType; 
		std::string strSlotType{ "default" }; 
		std::string strSlotName{ "slot name" }; 
		Node* node; 
		Editor* editor; 
		ImColor color{ 1.0f, 0.0f, 0.0f, 1.0f };
	};
	class Slot
	{
	public: 
		Slot(int id,
			SlotData data);
		~Slot(); 
	public: 
		virtual void render(ImGuiIO& io,
			const float& deltaTime);
		ax::NodeEditor::PinId& getId();
		ax::NodeEditor::PinKind& getPinKind();
		SlotType& getSlotType(); 
		std::string& getStrSlotType(); 
		Node* getNode(); 
		ImColor& getColor(); 
		void enableConnected(ax::NodeEditor::LinkId linkId);
		void disableConnected(ax::NodeEditor::LinkId link_remove);
		bool isCanMultiConnected();
		void setLinkId(ax::NodeEditor::LinkId linkId); 
		std::vector<ax::NodeEditor::LinkId>& getConnectedLinkId(); 
	private: 
		void DrawPinIcon(const ImVec2& size); 
	private: 
		//default
		std::string m_strSlotType{ "default" };
		std::string m_strNameFlowEnter{ "Flow" };
		std::string m_strNameFlowOut{ "Flow     " };
		std::string m_strName{ "" };
		ax::NodeEditor::PinId	m_Id; 
		ax::NodeEditor::PinKind m_PinKind;
		//ax::NodeEditor::LinkId  m_LinkId{ 0 };
		std::vector<ax::NodeEditor::LinkId> m_LinkIds{}; 
		int	m_IndexId{ 0 }; 
		Editor* m_Editor{ nullptr }; 
		Node* m_Node{ nullptr }; 
		ImColor m_Color{ 1.0f, 0.0f, 0.0f, 1.0f }; 
		ImColor m_InnerColor{ 0.0f,0.0f,0.0f,1.0f }; 
		SlotType m_SlotType{ SlotType::FLow };
		IconType m_IconType{ IconType::Flow }; 
		const int m_PinIconSize{ 24 };
		bool m_bIsConnected{ false }; 
		bool m_bIsMultiConnected{ false }; 
	};
}
#endif // !_SLOT_HPP_

