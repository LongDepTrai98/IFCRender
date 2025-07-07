#include "WindowFrame.hpp"
#include "AppMenubar.hpp"
#include "RenderCanvas.hpp"
#include "config/app_config.hpp"
#include "config/pannel_config.hpp"
#include "commands/AppCommandsHandler.hpp"
#include "ElementTreeCtrl.hpp"
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
		initCommand();
		initMenuBar();
		initScene();
		initTreeCtrl();
	}
	RenderCanvas* WindowFrame::getRenderCanvas()
	{
		return m_RenderCanvas.get();
	}
	ElementTreeCtrl* WindowFrame::getElementTreeCtrl()
	{
		return m_ElementTreeCtrl; 
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
		//wxTreeCtrl* tree = new wxTreeCtrl(treePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		long style = wxTR_HAS_BUTTONS
			^ wxTR_LINES_AT_ROOT
			^ wxTR_ROW_LINES
			^ wxTR_FULL_ROW_HIGHLIGHT
			^ wxTR_TWIST_BUTTONS; 
		m_ElementTreeCtrl = new ElementTreeCtrl(treePanel, wxDefaultPosition, wxDefaultSize, style);
		sizer->Add(m_ElementTreeCtrl, 1, wxEXPAND | wxALL, 5);
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
				//.Depth(24)
				.SampleBuffers(1)
				.Samplers(app_config::num_sampler)
				.Stencil(8)
				.FrameBuffersRGB()
				.PlatformDefaults()
				.EndList();
		}
		else
		{
			dispAttrs.PlatformDefaults()
				.RGBA()
				.DoubleBuffer()
				.Depth(24)
				.Stencil(8)
				.FrameBuffersRGB()
				.PlatformDefaults()
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
	void WindowFrame::initCommand()
	{
		if (!m_CommandHandler)
			m_CommandHandler = std::make_unique<AppCommandHandler>(this);
		/*BIND COMMAND*/
		Bind(wxEVT_MENU, &AppCommandHandler::OnOpenFile, m_CommandHandler.get(), wxID_OPEN);
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