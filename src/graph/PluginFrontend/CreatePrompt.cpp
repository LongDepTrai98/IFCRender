#include "CreatePrompt.hpp"
#include "../Extension/misc/cpp/imgui_stdlib.h"

namespace editor
{
	CreatePromptNode::CreatePromptNode(const int& id, NodeData data) : Node(id,data)
	{
	}
	CreatePromptNode::~CreatePromptNode()
	{
	}
	void CreatePromptNode::formOnFrame(ImGuiIO& io, const float& time)
	{
		for (auto& [key, component] : m_ui_component)
		{
			component->OnFrame(io,
				time); 
		}
		//static int   bar_data[8] = { 0,1,2,3,4,5,8,9 };
		//static float x_data[8] = { 0,1,2,3,4,5,8,9 };
		//static float y_data[8] = { 0,1,2,3,4,5,8,9 };
		/*	if (ImPlot::BeginPlot("My Plot", ImVec2(1000, 300)))
			{
				ImPlot::PlotBars("My Bar Plot", bar_data, 8);
				ImPlot::PlotLine("My Line Plot", x_data, y_data, 8);
				ImPlot::EndPlot();
			}*/
	}
	void CreatePromptNode::endFrame(ImGuiIO& io, const float& time)
	{
		for (auto& [key, component] : m_ui_component)
		{
			component->endFrame(io,
				time);
		}
	}
}