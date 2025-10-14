#pragma once
#ifndef _CREATE_PROMPT_NODE_HPP_
#define _CREATE_PROMPT_NODE_HPP_
#include "../Node/Node.hpp"
namespace editor
{
	class CreatePromptNode : public Node
	{
	public: 
		CreatePromptNode(const int& id,
			NodeData data); 
		~CreatePromptNode();
	public: 
		//override 
		void formOnFrame(ImGuiIO& io,
			const float& time) override; 
		void endFrame(ImGuiIO& io,
			const float& time) override; 
	};
}
#endif // !_CREATE_PROMPT_NODE_HPP_

