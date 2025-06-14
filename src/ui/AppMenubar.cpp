#include "AppMenubar.hpp"
#include "config/app_config.hpp"
namespace dragon
{
	AppMenubar::AppMenubar(wxFrame* main_window) : wxMenuBar(),
		m_Window(main_window)
	{
		wxMenu* menuFile = new wxMenu;
		menuFile->Append(component_id::ID_MENUBAR, "&Hello...\tCtrl+H",
			"Help string shown in status bar for this menu item");
		menuFile->AppendSeparator();
		menuFile->Append(wxID_EXIT);
		wxMenu* menuHelp = new wxMenu;
		menuHelp->Append(wxID_ABOUT);
		this->Append(menuFile, "&File");
		this->Append(menuHelp, "&Help");
		main_window->SetMenuBar(this);
	}
}