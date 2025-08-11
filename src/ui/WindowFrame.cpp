#include "WindowFrame.hpp"
#include "AppMenubar.hpp"
#include "RenderCanvas.hpp"
#include "MapRenderCanvas.hpp"
#include "config/app_config.hpp"
#include "config/pannel_config.hpp"
#include "commands/AppCommandsHandler.hpp"
#include "ElementTreeCtrl.hpp"
#include "core/Paths.hpp"
#include "core/utils/AppHelper.hpp"
#include "AppToolBar.hpp"
#include "input/input.hpp"
namespace dragon
{
	/*wxBEGIN_EVENT_TABLE(WindowFrame, wxFrame)
		EVT_SIZE(WindowFrame::OnSize)
		wxEND_EVENT_TABLE()*/
	WindowFrame::WindowFrame() : wxFrame(nullptr,
		wxID_ANY,
		app_config::app_name)
	{
		if (m_bIsMaximize)
		{
			this->Maximize(true);
		}
		initUIManager();
		initUI();
		initCommand();
		initMenuBar();
		initAppToolBar();
		initRenderCanvas();
		initMapRenderCanvas(); 
		initTreeCtrl();
		CreateStatusBar(2);
		SetStatusText("Welcome to wxWidgets!");
		Centre(wxBOTH);
		Bind(wxEVT_SIZE, &WindowFrame::OnResize, this);
	}
	RenderCanvas* WindowFrame::getRenderCanvas()
	{
		return m_RenderCanvas.get();
	}
	ElementTreeCtrl* WindowFrame::getElementTreeCtrl()
	{
		return m_ElementTreeCtrl;
	}
	void WindowFrame::initUI()
	{
		const std::string& iconAppPath = assets::Icons + "app.ico";
		wxIcon(iconAppPath, wxBITMAP_TYPE_ICO);
		this->SetIcon(wxIcon(iconAppPath, wxBITMAP_TYPE_ICO));
	}
	void WindowFrame::initUIManager()
	{
		long style = wxAUI_DOCKART_GRADIENT_TYPE | wxAUI_GRADIENT_NONE; 
		m_UIManager = std::make_unique<wxAuiManager>(this, wxAUI_GRADIENT_NONE);
		wxAuiDockArt* art = m_UIManager->GetArtProvider();
		customDockArt* default_doc_art = new customDockArt();
		default_doc_art->setStyle(); 
		m_UIManager->SetArtProvider(default_doc_art); 
		m_UIManager->Update(); 
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
		long style = wxTR_HAS_BUTTONS | wxTR_NO_LINES | wxTR_TWIST_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT | wxBORDER_SUNKEN;
		m_ElementTreeCtrl = new ElementTreeCtrl(treePanel, wxDefaultPosition, wxDefaultSize, style);
		sizer->Add(m_ElementTreeCtrl, 1, wxEXPAND | wxALL, 1);
		treePanel->SetSizer(sizer);
		const std::string& checkedPath = assets::Icons + "ElementTree.ico";
		m_UIManager->AddPane(treePanel, panel_config::tree_ctrl_panel_info.Icon(AppHelper::loadBitmapBundle(checkedPath, wxBITMAP_TYPE_ICO)));
		m_UIManager->Update();
	}
	void WindowFrame::initAppToolBar()
	{
		m_AppToolBar = std::make_unique<AppToolBar>(this);
		m_UIManager->AddPane(m_AppToolBar->m_ToolBar,
			wxAuiPaneInfo()
			.Name("toolbar")
			.Caption("Toolbar")
			.ToolbarPane()
			.Top()             
			.DockFixed()        
			.Gripper(false)); 
	}
	void WindowFrame::initRenderCanvas()
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
			const std::string& checkedPath = assets::Icons + "scene.ico";
			m_UIManager->AddPane(m_RenderCanvas.get(), panel_config::scene_view_panel_info.Icon(AppHelper::loadBitmapBundle(checkedPath, wxBITMAP_TYPE_ICO)));
			m_UIManager->Update();
		}
	}
	void WindowFrame::initMapRenderCanvas()
	{
		//wxGLAttributes dispAttrs;
		//if (app_config::enable_msaa)
		//{
		//	dispAttrs.PlatformDefaults()
		//		.RGBA()
		//		.DoubleBuffer()
		//		//.Depth(24)
		//		.SampleBuffers(1)
		//		.Samplers(app_config::num_sampler)
		//		.Stencil(8)
		//		.FrameBuffersRGB()
		//		.PlatformDefaults()
		//		.EndList();
		//}
		//else
		//{
		//	dispAttrs.PlatformDefaults()
		//		.RGBA()
		//		.DoubleBuffer()
		//		.Depth(24)
		//		.Stencil(8)
		//		.FrameBuffersRGB()
		//		.PlatformDefaults()
		//		.EndList();
		//}
		//if (!wxGLCanvas::IsDisplaySupported(dispAttrs))
		//{
		//	throw std::exception("glCanvans not support display attribute");
		//}
		if (!m_MapRenderCanvas)
		{
			m_MapRenderCanvas = std::make_unique<MapRenderCanvas>(this,
				/*dispAttrs,*/
				m_RenderCanvas->getRenderContext());
			m_MapRenderCanvas->m_Context = m_RenderCanvas->getRenderContext(); 
			const std::string& checkedPath = assets::Icons + "scene.ico";
			m_UIManager->AddPane(m_MapRenderCanvas.get(), panel_config::map_view_panel_info.Icon(AppHelper::loadBitmapBundle(checkedPath, wxBITMAP_TYPE_ICO)));
			m_UIManager->Update();
			m_RenderCanvas->SetCurrent(*m_RenderCanvas->getRenderContext()); 
		}
	}
	void WindowFrame::initCommand()
	{
		if (!m_CommandHandler)
			m_CommandHandler = std::make_unique<AppCommandHandler>(this);
		/*BIND COMMAND*/
		Bind(wxEVT_MENU, &AppCommandHandler::OnOpenFile, m_CommandHandler.get(), wxID_OPEN);
		Bind(wxEVT_ICONIZE, &AppCommandHandler::OnIconize, m_CommandHandler.get());
		Bind(wxEVT_TOOL, &AppCommandHandler::OnToolbarClick, m_CommandHandler.get());
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
	void WindowFrame::OnCallbackToolbarCommand(ToolBarData& data)
	{
		if (m_RenderCanvas)
			m_RenderCanvas->OnCallbackToolbarCommand(data);
	}
	void WindowFrame::OnResize(wxSizeEvent& event)
	{
		event.Skip();
	}
	wxAuiToolBar* WindowFrame::getCustomToolBar()
	{
		return m_AppToolBar->m_ToolBar; 
	}
}