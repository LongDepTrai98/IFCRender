#include "WindowFrame.hpp"
#include "config/app_config.hpp"
#include "config/pannel_config.hpp"
#include "RenderCanvas.hpp"
namespace dragon
{
	wxBEGIN_EVENT_TABLE(WindowFrame, wxFrame)
		EVT_SIZE(WindowFrame::OnSize)
	wxEND_EVENT_TABLE()
	WindowFrame::WindowFrame() : wxFrame(nullptr,
		wxID_ANY,
		app_config::app_name)
	{
		initUIManager(); 
		initMenuBar(); 
		initScene(); 
		initTreeCtrl(); 
	}
	void WindowFrame::initUIManager()
	{
		m_UIManager = std::make_unique<wxAuiManager>(this); 
	}
	void WindowFrame::initMenuBar()
	{
		if (!m_MenuBar)
		{
			m_MenuBar = new wxMenuBar(); 
		}
		wxMenu* menuFile = new wxMenu;
		menuFile->Append(dragon::component_id::ID_MENUBAR, "&Hello...\tCtrl+H",
			"Help string shown in status bar for this menu item");
		menuFile->AppendSeparator();
		menuFile->Append(wxID_EXIT);
		wxMenu* menuHelp = new wxMenu;
		menuHelp->Append(wxID_ABOUT);
		m_MenuBar->Append(menuFile, "&File");
		m_MenuBar->Append(menuHelp, "&Help");
		SetMenuBar(m_MenuBar);
	}
	void WindowFrame::initTreeCtrl()
	{
		// create panel store tree ctrl
		wxPanel* treePanel = new wxPanel(this);
		wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
		wxTreeCtrl* tree = new wxTreeCtrl(treePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		wxTreeItemId root = tree->AddRoot("Root");
		tree->AppendItem(root, "Child 1");
		tree->AppendItem(root, "Child 2");
		tree->Expand(root);
		sizer->Add(tree, 1, wxEXPAND | wxALL, 5);
		treePanel->SetSizer(sizer);
		m_UIManager->AddPane(treePanel, panel_config::tree_ctrl_panel_info);
		m_UIManager->Update(); 
	}
	void WindowFrame::initScene()
	{
		wxGLAttributes dispAttrs;
		if (m_bIsEnbleMSAA)
		{
			dispAttrs.PlatformDefaults()
				.RGBA()
				.DoubleBuffer()
				.Depth(24)
				.Defaults()
				.SampleBuffers(1)
				.Samplers(m_sampler)
				.EndList();
		}
		else
		{
			dispAttrs.PlatformDefaults()
				.Defaults()
				.EndList();
		}
		if (!wxGLCanvas::IsDisplaySupported(dispAttrs))
		{
			throw std::exception("glCanvans not support display attribute"); 
		}

		if (!m_RenderCanvas)
		{
			m_RenderCanvas = std::make_unique<RenderCanvas>(this,
				dispAttrs); 
			m_UIManager->AddPane(m_RenderCanvas.get(), panel_config::scene_view_panel_info);
			m_UIManager->Update(); 
		}
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
	void WindowFrame::OnSize(wxSizeEvent& event)
	{
		event.Skip(); 
	}
}