#ifndef _NODE_HPP_
#define _NODE_HPP_

#include "Slot.hpp"
#include <nlohmann/json.hpp>
#include <unordered_map>
#include "../UIComponent/UIComponent.hpp"
namespace editor
{
	//Interface of Node 
	class Editor; 

	enum class NodeStatus : int { Create, Running, Completed, Error };


	namespace node_border_color
	{
		static ImColor node_border{ 1.0f,1.0f,1.0f,0.77f }; 
		static ImColor node_running{ 1.0f, 1.0f, 0.0f, 0.77f };
		static ImColor node_error{ 1.0f, 0.0f, 0.0f, 0.77f };
		static ImColor node_completed{ 0.0f, 1.0f, 0.0f, 0.77f };
	}

	struct NodeData
	{
		std::string szId{ "" }; 
		std::string name{ "" }; 
		std::string strNodeType{ "" };
		std::string strUniqueId{ "" }; 
		std::string strDescription{ "" }; 
		ImTextureID textureButtonExcute{ nullptr }; 
		ImTextureID textureNodeIcon{ nullptr }; 
		ImVec2 postion{}; 
		Editor* editor{ nullptr };
		nlohmann::json inputPins{};
		nlohmann::json outputPins{};
		nlohmann::json form{}; 
		nlohmann::json formData{}; 
	};

	enum class NodeType : ImU32 { FLow, Value }; 
	class WFVariant; 

	class Node
	{
	public: 
		Node(const int& id,
			NodeData data);
		~Node(); 
	public: 
		virtual void formOnFrame(ImGuiIO& io,
			const float& time);
		virtual void endFrame(ImGuiIO& io,
			const float& time);
		void OnFrame(ImGuiIO& io,
			const float& time); 
		int getIndexSlotInput(ax::NodeEditor::PinId idSlot); 
		int getIndexSlotOutput(ax::NodeEditor::PinId idSlot);
		ax::NodeEditor::PinId getInputPinIdWithIndexPin(const int& index); 
		ax::NodeEditor::PinId getOutputPinIdWithIndexPin(const int& index); 
		ImVec2& getPostion(); 
		std::string& getStrId(); 
		//std::string& getStrUniqueID() override; 
		std::vector<Slot*>& getInput(); 
		std::vector<Slot*>& getOutput(); 
		std::unique_ptr<UIComponent> createUIComponent(nlohmann::json& component_json);
		NodeStatus& getStatus(); 
		nlohmann::json serializeFormInput();
		void upateUniqueIdNode(); 
		bool isNotUniqueID(); 
		void setValueForComponent(nlohmann::json& element_json);
		const std::string& getStrUniqueID(); 
	private: 
		void createSlotDirect(); 
		//void running() override; 
		//void complete() override; 
		//void error() override;
		//void setInput(std::vector<std::shared_ptr<backend::WFVariant>> lstVariant) override;
		//void setOutput(std::vector<std::shared_ptr<backend::WFVariant>> lstVariant) override;
	private: 
		//callback 
		static int InputTextUniqueIDCallback(ImGuiInputTextCallbackData* data); 
	protected: 
		std::string m_StrName{ "SimpleNode" }; 
		std::string m_StrUniqueID{ "" }; 
		std::string m_StrDescription{ "Node description" }; 
		std::string m_StrButtonExcute{ "Excute Node" }; 
		NodeStatus m_Status{ NodeStatus::Create };
		ImVec2 m_Postion{0,0}; 
		ImVec2 m_DummyVec{ 0,0 }; 
		std::vector<Slot*> m_Inputs; 
		std::vector<Slot*> m_Outputs;
		std::string m_strId{""};
		NodeType m_nodeType{ NodeType::Value }; 
		ax::NodeEditor::NodeId m_Id;
		Editor* m_Editor{ nullptr };
		ImVec2      m_HeaderMin{0.0f,0.0f};
		ImVec2		m_HeaderMinSlotSpace{ 0.0f,0.0f }; 
		ImVec2      m_HeaderMax{0.0f,0.0f};
		float		m_XDistance{ 0.0f }; 
		bool		m_bIsFirstFrameNode{ true }; 
		bool		m_bIsDoToolTip{ false }; 
		bool		m_bIsDoToolTipButtonExcute{ false }; 
		bool		m_bIsNotUniqueId{ false }; 
		ImColor		m_CurrentColorNodeBorder{ 1.0f,1.0f,1.0f,1.0f }; 
		ImVec4		m_ColorTextNotUniqueID{ 1.0f,0.0f,0.0f,1.0f }; 
		ImVec4		m_ColorNormalText{ 1.0f,1.0f,1.0f,1.0f };
		ImTextureID m_Texture_Button_Excute{ nullptr };
		ImTextureID m_Texture_Node_Icon{ nullptr };
		//UI component 
		std::unordered_map<std::string, std::unique_ptr<UIComponent>> m_ui_component; 
	};
}
#endif // !_NODE_HPP_
