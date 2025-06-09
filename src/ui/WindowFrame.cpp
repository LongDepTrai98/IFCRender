#include "WindowFrame.hpp"
#include "config/app_config.hpp"
#include <wx/wx.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/page.h>
#include <wx/ribbon/panel.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/treectrl.h>
namespace viewer
{
	WindowFrame::WindowFrame() : wxFrame(nullptr,
		wxID_ANY,
		app_config::app_name)
	{
		initMenuBar(); 
		m_mgr.SetManagedWindow(this);

		// Tạo panel chứa TreeCtrl
		wxPanel* treePanel = new wxPanel(this);
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

		wxTreeCtrl* tree = new wxTreeCtrl(treePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
			wxTR_DEFAULT_STYLE | wxTR_HAS_BUTTONS );
		wxTreeItemId root = tree->AddRoot("Root");
		tree->AppendItem(root, "Child 1");
		tree->AppendItem(root, "Child 2");
		tree->Expand(root);

		sizer->Add(tree, 1, wxEXPAND | wxALL, 5);
		treePanel->SetSizer(sizer);

		// Đăng ký panel vào AUI manager như pane dockable bên trái
		m_mgr.AddPane(treePanel, wxAuiPaneInfo().
			Name("tree").Caption("Project Tree").
			Left().CloseButton(true).MaximizeButton(true).PinButton(true).
			MinSize(200, 300).
			Dockable(true));

		m_mgr.Update();
	}
	void WindowFrame::initMenuBar()
	{
		if (!m_MenuBar)
		{
			m_MenuBar = new wxMenuBar(); 
		}
		wxMenu* menuFile = new wxMenu;
		menuFile->Append(viewer::component_id::ID_MENUBAR, "&Hello...\tCtrl+H",
			"Help string shown in status bar for this menu item");
		menuFile->AppendSeparator();
		menuFile->Append(wxID_EXIT);
		wxMenu* menuHelp = new wxMenu;
		menuHelp->Append(wxID_ABOUT);
		m_MenuBar->Append(menuFile, "&File");
		m_MenuBar->Append(menuHelp, "&Help");
		SetMenuBar(m_MenuBar);
	}
	void WindowFrame::OnHello(wxCommandEvent& event)
	{
		wxLogMessage("Hello world from wxWidgets!");
	}
	void WindowFrame::OnExit(wxCommandEvent& event)
	{
		Close(true);
	}
	void WindowFrame::OnAbout(wxCommandEvent& event)
	{
		wxMessageBox("This is a wxWidgets Hello World example",
			"About Hello World", wxOK | wxICON_INFORMATION);
	}
}