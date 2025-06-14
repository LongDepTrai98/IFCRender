#include "WindowFrame.hpp"
#include "AppMenubar.hpp"
#include "RenderCanvas.hpp"
#include "config/app_config.hpp"
#include "config/pannel_config.hpp"
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
		if (!m_AppMenuBar)
		{
			m_AppMenuBar = new AppMenubar(this);
		}
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
		if (app_config::enable_msaa)
		{
			dispAttrs.PlatformDefaults()
				.RGBA()
				.DoubleBuffer()
				.Depth(24)
				.SampleBuffers(1)
				.Samplers(app_config::num_sampler)
				.EndList();
		}
		else
		{
			dispAttrs.PlatformDefaults()
				.RGBA()
				.DoubleBuffer()
				.Depth(24)
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