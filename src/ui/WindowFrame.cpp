#include "WindowFrame.hpp"
#include "AppMenubar.hpp"
#include "IGLCanvas.hpp"
#include "BimRenderCanvas.hpp"
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
		initMapRenderCanvas();
		//initBimRenderCanvas();
		initTreeCtrl();
		//CreateStatusBar(2);
		//SetStatusText("Welcome to wxWidgets!");
		Centre(wxBOTH);
		m_UIManager->Update(); 
		Bind(wxEVT_SIZE, &WindowFrame::OnResize, this);
	}
	IGLCanvas* WindowFrame::getCanvasWithName(const std::string& name)
	{
		for (auto& canvas : m_Canvas)
		{
			if (canvas->getCanvasName() == name)
			{
				return canvas.get(); 
			}
		}
		return nullptr; 
	}
	ElementTreeCtrl* WindowFrame::getElementTreeCtrl()
	{
		return m_ElementTreeCtrl;
	}
	void WindowFrame::initUI()
	{
		const std::string iconAppPath = assets::Icons + "app.ico";
		this->SetIcon(wxIcon(iconAppPath, wxBITMAP_TYPE_ICO));
	}
	void WindowFrame::initUIManager()
	{
		m_UIManager = std::make_unique<wxAuiManager>(this);
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
			.Gripper(false)); 
	}
	void WindowFrame::initBimRenderCanvas()
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
		std::unique_ptr<BimRenderCanvas> bim_render_canvas = std::make_unique<BimRenderCanvas>(this, dispAttrs); 
		const std::string& checkedPath = assets::Icons + "scene.ico";
		m_UIManager->AddPane(bim_render_canvas.get(), panel_config::scene_view_panel_info.Icon(AppHelper::loadBitmapBundle(checkedPath, wxBITMAP_TYPE_ICO)));
		m_Canvas.emplace_back(std::move(bim_render_canvas)); 
	}
	void WindowFrame::initMapRenderCanvas()
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
		const std::string& checkedPath = assets::Icons + "scene.ico";
		std::unique_ptr<MapRenderCanvas> map_render_canvas = std::make_unique<MapRenderCanvas>(this, dispAttrs); 
		m_UIManager->AddPane(map_render_canvas.get(), panel_config::map_view_panel_info.Icon(AppHelper::loadBitmapBundle(checkedPath, wxBITMAP_TYPE_ICO)));
		m_Canvas.emplace_back(std::move(map_render_canvas)); 
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
	void WindowFrame::OnCallbackToolbarCommand(ToolBarData& data)
	{
		for (auto& canvas : m_Canvas)
		{
			canvas->OnCallbackToolbarCommand(data); 
		}
	}
	void WindowFrame::OnResize(wxSizeEvent& event)
	{
		event.Skip();
	}
	wxAuiToolBar* WindowFrame::getCustomToolBar()
	{
		return m_AppToolBar->m_ToolBar; 
	}
	std::vector<std::unique_ptr<IGLCanvas>>& WindowFrame::getCanvas()
	{
		return m_Canvas; 
	}
}