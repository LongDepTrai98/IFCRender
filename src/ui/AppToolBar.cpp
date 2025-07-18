#include "AppToolBar.hpp"
#include "core/Paths.hpp"
#include "core/utils/AppHelper.hpp"
#include "resource.hpp"
namespace dragon
{
	AppToolBar::AppToolBar(wxToolBar* toolbar) : m_ToolBar(toolbar)
	{
		wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_SPECIFIED);
		m_ToolBar->SetExtraStyle(m_ToolBar->GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES);
		AddTool(); 
		m_ToolBar->Realize();
	}
	void AppToolBar::AddTool()
	{
		/*FILE TOOL*/
		m_ToolBar->AddTool(wxID_OPEN, 
			"Open",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_OpenFolder,wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_NORMAL,
			"Open file", 
			"This is help for Open file tool");
		m_ToolBar->AddSeparator();
		/*EDITOR TOOL*/
		m_ToolBar->AddTool((int)ID_EVENT::TOOL_PIVOT,
			"Pivot",
			AppHelper::loadBitmapBundle(assets::ButtonToolBar_SetPivot,wxBITMAP_TYPE_PNG),
			wxNullBitmap,
			wxITEM_CHECK,
			"Select Pivot",
			"This is help for select pivot"); 
	}
}