#pragma once
#ifndef _EDITOR_HPP_
#define _EDITOR_HPP_
#include <imgui_node_editor.h>
#include "../Graph/Node/Node.hpp"
#include "Node/Slot.hpp"
#include <map>
#include <set>
#include <nlohmann/json.hpp>
namespace ed = ax::NodeEditor;
class NodeApplication;
namespace editor
{
	enum class EditorStyle : ImU32 { DARK,LIGHT };
	enum class EditorStatus : int{Running,Completed};
	struct LinkInfo
	{
		ed::LinkId Id;
		ed::PinId  sourceId;
		ed::PinId  targetId;
		std::string m_strUniqueIdSource{ "" };
		std::string m_strUniqueIdTarget{ "" };
		int indexSourcePin{ 0 };
		int indexTargetPin{ 0 };
	};

	class Editor
	{
	public: 
		Editor(NodeApplication* application);
		~Editor();
	public: 
		void createEditorContext(const std::string& jsonPath); 
		void editorOnFrame(const float& time); 
		void destroy(); 
		void getNextUniqueId(int& pinkId);
		NodeApplication* getApplication();
		bool iSFirstFrame(); 
		Slot* createSlot(SlotData slotData);
		Slot* getSlot(const int& slotId); 
		Node* createNode(const std::string& idNodeSchema,
			const ImVec2& postion, 
			const std::string& strUniqueId, 
			const nlohmann::json& formData);
		Node* getNode(ed::NodeId id); 
		Node* getNodeWithStrUniqueId(const std::string& uniqueId); 
		void setStyle(EditorStyle style);
		nlohmann::json serializeWorkflowGraph(); 
		bool deserializeWorkflowGraph(nlohmann::json& graph_json); 
		void clear(); 
		void removeLink(const ed::LinkId& idLink); 
		bool checkIdIsUnique(const int& nodeId,
			const std::string& str); 
		nlohmann::json getJsonSchema(); 
		std::map<int, std::unique_ptr<Node>>& getNodes(); 
		void setStatus(EditorStatus status);
		EditorStatus& getStatus(); 
	private: 
		void deleteNode(ed::NodeId& nodeId);
		void deleteSlot(ed::PinId& pinId); 
		void loadSchema(const std::string& path); 
		void handleLink(); 
		void showLabel(const std::string& label,
			const ImColor& color); 
		void showFlow(); 
		void handleHover(); 
		void drawOrdinals(); 
	private: 
		//context editor 
		ed::EditorContext* m_Context{ nullptr };
		ed::PinId          m_PointerUniquePinkId{};
		//text 
		std::string m_strEditorName{ "Node Editor" }; 
		std::string m_strCreateNewLink{ "+ Create New Link" };
		std::string m_strRejectNewLink{ "x Incompatible Pin Type" }; 
		int m_UniqueId{ 1 }; 
		//int m_UniqueLinkId{ 1 }; 
		bool m_bIsFirstFrame{ true };
		bool m_bIsShowFPS{ false };
		bool m_bIsShowDemoWindow{ false }; 
		bool m_bShowOrdinals{ true }; 
		float m_linkThickness{ 3.0f };
		//default dark 
		EditorStyle m_currentStyle{ EditorStyle::DARK }; 
		EditorStatus m_status{ EditorStatus::Completed }; 
		ImColor m_colorAccecptLink{ 0.0f, 1.0f, 0.0f, 1.0f };
		ImColor m_colorRejectLink{ 1.0f,0.0f,0.0f,1.0f }; 
		ImColor m_colorLabel{ 32, 45, 32, 180 }; 
		ImColor m_colorBGDark{ 0.0f,0.0f,0.0f,0.5f }; 
		ImColor m_colorBGLight{ 1.0f,1.0f,1.0f,0.884313798f };
		nlohmann::json m_jsonSchema{}; 
		//ImVector<LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
		std::map<int,LinkInfo>   m_Links;                // List of live links. It is dynamic unless you want to create read-only view over nodes.
		std::map<int,std::unique_ptr<Node>> m_Nodes;
		std::map<int, std::unique_ptr<Slot>> m_mapSlot{};
		NodeApplication* m_Application{ nullptr };
		//Editor data 
	};
}
#endif // !_EDITOR_HPP_

