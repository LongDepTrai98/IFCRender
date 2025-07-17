#include "AppToolBar.hpp"
#include "core/Paths.hpp"
#include "core/utils/AppHelper.hpp"
namespace dragon
{
	AppToolBar::AppToolBar(wxToolBar* toolbar) : m_ToolBar(toolbar)
	{
		wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_SPECIFIED);
		m_ToolBar->SetExtraStyle(m_ToolBar->GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES);

		/*OPEN TOOL*/
		const std::string& checkedPath = assets::ButtonToolBar_OpenFolder;
		wxIcon icon(checkedPath, wxBITMAP_TYPE_PNG);
		wxSize iconSize(icon.GetWidth(), icon.GetHeight());

		m_ToolBar->AddTool(wxID_OPEN, "Open",
			wxBitmapBundle::FromImpl(new FixedSizeImpl(iconSize, icon)), wxNullBitmap, wxITEM_NORMAL,
			"Open file", "This is help for Open file tool");

		m_ToolBar->AddSeparator();
		m_ToolBar->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
		m_ToolBar->Realize();
	}
}