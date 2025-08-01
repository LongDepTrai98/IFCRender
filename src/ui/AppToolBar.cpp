#include "AppToolBar.hpp"
#include "core/Paths.hpp"
#include "core/utils/AppHelper.hpp"
#include "resource.hpp"
namespace dragon
{
	AppToolBar::AppToolBar(wxWindow* main)
	{
		wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_SPECIFIED);
		m_ToolBar = new wxAuiToolBar(main, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_GRIPPER | wxAUI_TB_HORIZONTAL);
		AddTool();
		m_ToolBar->Realize();
	}
	void AppToolBar::AddTool()
	{
		/*FILE TOOL*/
		m_ToolBar->AddTool(wxID_OPEN,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_OpenFolder, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_NORMAL,
			"Open file",
			"This is help for Open file tool",
			NULL);
		m_ToolBar->AddSeparator();
		/*EDITOR TOOL*/
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_PIVOT,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_SetPivot, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_CHECK,
			"Select Pivot",
			"This is help for select pivot", 
			NULL);
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_HOVER,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_Hover, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_CHECK,
			"Select Pivot",
			"This is help for select pivot", 
			NULL);
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_MULTI_SELECT,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_MultiSelect, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_CHECK,
			"Multi Select Mode",
			"This is tool for multi select object", 
			NULL);
		m_ToolBar->AddSeparator();
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_DRAW_EDGE,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_DrawEdge, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_NORMAL,
			"Draw Edge",
			"This is help for draw object edge", 
			NULL);
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_DRAW_GRID,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_DrawGrid, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_NORMAL,
			"Draw Grid",
			"This is help for draw Grid", 
			NULL);
		m_ToolBar->AddSeparator();
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_SWITCH_MODE_RENDER,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_Debug, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_CHECK,
			"Debug mode",
			"Debug mode", 
			NULL);
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_CLEAR_SCENE,
			"",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_Debug, wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_CHECK,
			"Debug mode",
			"Debug mode", 
			NULL);
	}
}