#include "AppMenubar.hpp"
#include "config/app_config.hpp"
namespace dragon
{
	AppMenubar::AppMenubar(wxFrame* main_window) : wxMenuBar(0),
		m_Window(main_window)
	{
		wxMenu* menuFile = new wxMenu;
		menuFile->Append(wxID_OPEN, "&Open",
			"Open File");
		menuFile->AppendSeparator();
		menuFile->Append(wxID_EXIT);
		wxMenu* menuHelp = new wxMenu;
		menuHelp->Append(wxID_ABOUT);
		this->Append(menuFile, "&File");
		this->Append(menuHelp, "&Help");
		main_window->SetMenuBar(this);
	}
}