#include "MapRenderCanvas.hpp"
#include "map/Win32View.hpp"
#include "map/Win32RenderFrontEnd.hpp"
#include <mbgl/gfx/backend.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include "core/lock/ContextLock.hpp"
#include "map/example_custom_drawable_style_layer.hpp"
#include "map/CesiumLayer.hpp"
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer_impl.hpp>
#include "resource.hpp"
#include "input/input.hpp"
#include "core/utils/AppHelper.hpp"
#include "WindowFrame.hpp"
#include "core/node/MapElementTree.hpp"
#include "tools/Gizmo.hpp"
namespace dragon
{
	MapRenderCanvas::MapRenderCanvas(wxWindow* parent,
		const wxGLAttributes& canvasAttrs) :
		IGLCanvas(parent,
			canvasAttrs)
	{
		m_Name = "map-canvas"; 
		ctxAttrs.PlatformDefaults()
			.CoreProfile()
			.OGLVersion(3, 0)
			.EndList();
		initGLContext(); 
		m_ContextLock->lock();
		bindFunction(); 
		initContextMap();
		initUI(); 
		initCallback(); 
		m_ContextLock->unlock(); 
	}
	MapRenderCanvas::~MapRenderCanvas()
	{
	}
	void MapRenderCanvas::OnCallbackToolbarCommand(ToolBarData& data)
	{
		using namespace mbgl::style;
		using namespace mbgl::style::expression::dsl;

		if (data.event.GetId() == (int)ID_EVENT::TOOL_OPEN_EDIT_MODE)
		{
			/*GET CUSTOM 3D STYLE*/
			m_ContextLock->lock(); 
			auto host = getCustomDrawableStyleLayerHost("Example-Bim-Layer");
			if (host)
			{
				host->openEditMode(data.bIsCheck); 
			}
			data.bIsCheck ? mode = MODE::EDIT : mode = MODE::VIEW; 
			m_ContextLock->unlock(); 
		}

		if (data.event.GetId() == (int)ID_EVENT::TOOL_PROJECTION)
		{
			/*ADD BIM TO MAP*/
			WindowFrame* main_frame = static_cast<WindowFrame*>(m_parent); 
			MainViewPort* viewport_bim = AppHelper::getMainBimViewPortScene(main_frame);
			/*GET MODEL BIM*/
			auto model = viewport_bim->getScene()->getObjectByName("model"); 
			ThreeDCustomDrawableStyleLayerHost* custom_host{ nullptr }; 
			if (model)
			{
				/*CLONE*/
				MLN_TRACE_FUNC();
				std::shared_ptr<threepp::Object3D> clone_model = model->clone(true); 
				clone_model->matrixAutoUpdate = false;
				mbgl::style::Style& style = m_Map->getStyle();
				const std::string identifier = "Example-Bim-Layer";
				const auto& existingLayer = style.getLayer(identifier);
				if (!existingLayer) {
					style.addLayer(std::make_unique<mbgl::style::CustomDrawableLayer>(
						identifier, std::make_unique<ThreeDCustomDrawableStyleLayerHost>()));
				}; 
				custom_host = getCustomDrawableStyleLayerHost("Example-Bim-Layer");
				custom_host->addBim(clone_model); 
			}
		}
		if (data.event.GetId() == (int)ID_EVENT::TOOL_ADD_CESIUM_LAYER)
		{
			MLN_TRACE_FUNC();
			mbgl::style::Style& style = m_Map->getStyle();
			const std::string identifier = "Test-Cesium";
			const auto& existingLayer = style.getLayer(identifier);
			if (!existingLayer) {
				style.addLayer(std::make_unique<mbgl::style::CustomDrawableLayer>(
					identifier, std::make_unique<CesiumDrawableStyleLayerHost>()));
				m_Map->jumpTo(mbgl::CameraOptions()
					.withCenter(mbgl::LatLng{ 10.759637476519089, 106.69353758860171 })
					.withZoom(16)
					.withBearing(0.0)
					.withPitch(0.0));
			};
		}
	}
	void MapRenderCanvas::initContextMap()
	{
		auto MapboxConfiguration = mbgl::TileServerOptions::MapboxConfiguration();
		mbgl::ResourceOptions resourceOptions;
		resourceOptions
			.withApiKey("pk.eyJ1IjoiYW5odHVzeHl6IiwiYSI6ImNsdng4ZGp3ZTA2aDgyaWw3ZnM2NXJhcjcifQ.OV7YSJsVT8zY-L4tozXaVw")
			.withTileServerOptions(MapboxConfiguration);
		mbgl::ClientOptions clientOptions;
		auto orderedStyles = MapboxConfiguration.defaultStyles();
		m_Backend = std::make_unique<editor::Win32View>(this,
			true,
			true,
			resourceOptions,
			clientOptions);
		m_FrontEnd = std::make_unique<editor::Win32RenderFrontEnd>(
			std::make_unique<mbgl::Renderer>(m_Backend->getRendererBackend(), m_Backend->getPixelRatio()), *m_Backend.get());
		m_Map = std::make_unique<mbgl::Map>(*m_FrontEnd,
			*m_Backend,
			mbgl::MapOptions().withSize(m_Backend->getSize()).withPixelRatio(m_Backend->getPixelRatio()),
			resourceOptions,
			clientOptions);
		m_Backend->setMap(m_Map.get());
		m_Map->getStyle().loadURL(orderedStyles[5].getUrl());
		m_Map->jumpTo(mbgl::CameraOptions()
			.withCenter(mbgl::LatLng{ 10.770091878555718, 106.69353758860171 })
			.withZoom(16)
			.withBearing(0.0)
			.withPitch(0.0));
		/*set callback*/
		auto callback_finsish_loading_style = [&]() {
			if (!m_Map) return; 
			mbgl::style::Style& style = m_Map->getStyle(); 
			std::shared_ptr<MapLayerTree> tree = std::make_shared<MapLayerTree>(); 
			tree->create(style);
			WindowFrame* window_frame = static_cast<WindowFrame*>(m_parent);
			auto element_tree = AppHelper::getMainTreeCtrl(window_frame);
			element_tree->m_umap_callback["map"].m_GetData_Item_Callback = m_GetData_Item_Callback;
			element_tree->m_umap_callback["map"].m_ToggleStateCallBackRecursively = m_Toggle_Components_Callback; 
			element_tree->setData(std::move(tree),"map");
		};
		m_Backend->finishLoadingStyleCallback(callback_finsish_loading_style); 
	}
	void MapRenderCanvas::initUI()
	{
		using namespace mbgl::style;
		using namespace mbgl::style::expression::dsl;
		int padding = 1;
		int buttonSize = 30; 
		int posYButton = 0; 
		wxButton* btnZoomIn = new wxButton(this, wxID_ANY, "+", wxPoint(10, 10), wxSize(buttonSize, buttonSize));
		posYButton = btnZoomIn->GetPosition().y + padding + buttonSize;
		wxButton* btnZoomOut = new wxButton(this, wxID_ANY, "-", wxPoint(10, posYButton), wxSize(30, 30));
		posYButton = btnZoomOut->GetPosition().y + padding + buttonSize;
		wxButton* btn2D = new wxButton(this, wxID_ANY, "2D", wxPoint(10, posYButton), wxSize(30, 30));
		posYButton = btn2D->GetPosition().y + padding + buttonSize;
		wxButton* btn3D = new wxButton(this, wxID_ANY, "3D", wxPoint(10, posYButton), wxSize(30, 30));
		posYButton = btn3D->GetPosition().y + padding + buttonSize;
		wxButton* bimBtn = new wxButton(this, wxID_ANY, "Bim", wxPoint(10, posYButton), wxSize(30, 30));
		posYButton = bimBtn->GetPosition().y + padding + buttonSize;
		wxButton* gridBtn = new wxButton(this, wxID_ANY, "Grid", wxPoint(10, posYButton), wxSize(30, 30));
		posYButton = gridBtn->GetPosition().y + padding + buttonSize;
		wxButton* transButton = new wxButton(this, wxID_ANY, "Trans", wxPoint(10, posYButton), wxSize(30, 30));
		posYButton = transButton->GetPosition().y + padding + buttonSize;
		wxButton* rotateButton = new wxButton(this, wxID_ANY, "Rot", wxPoint(10, posYButton), wxSize(30, 30));
		posYButton = rotateButton->GetPosition().y + padding + buttonSize;
		transButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
			getCustomDrawableStyleLayerHost("Example-Bim-Layer")->getGizmo()->switchMode(Gizmo::MODE::TRANSLATE);
		}); 
		rotateButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
			getCustomDrawableStyleLayerHost("Example-Bim-Layer")->getGizmo()->switchMode(Gizmo::MODE::ROTATE);
		});
		gridBtn->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event) {
			MLN_TRACE_FUNC();
			if (m_Map)
			{
				if (m_Map->getDebug() != mbgl::MapDebugOptions::NoDebug)
				{
					m_Map->setDebug(mbgl::MapDebugOptions::NoDebug);
				}
				else
				{
					m_Map->setDebug(mbgl::MapDebugOptions::Collision
						| mbgl::MapDebugOptions::Timestamps
						| mbgl::MapDebugOptions::TileBorders
						| mbgl::MapDebugOptions::ParseStatus);
				}
			}
			}); 
		
		bimBtn->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
			{
				MLN_TRACE_FUNC();
			
			}); 

		btn3D->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
			{
				MLN_TRACE_FUNC();
				using namespace mbgl::style;
				using namespace mbgl::style::expression::dsl;
				mbgl::style::Style& style = m_Map->getStyle(); 
				if (!style.getSource("composite")) {
					return;
				}
				if (auto layer = style.getLayer("3d-buildings")) {
					auto visible = layer->getVisibility(); 
					if (visible == VisibilityType::Visible)
					{
						layer->setVisibility(VisibilityType::None);
					}
					else
					{
						layer->setVisibility(VisibilityType::Visible);
					}
					return;
				}
				auto extrusionLayer = std::make_unique<FillExtrusionLayer>("3d-buildings", "composite");
				extrusionLayer->setSourceLayer("building");
				extrusionLayer->setMinZoom(15.0f);
				extrusionLayer->setFilter(Filter(eq(get("extrude"), literal("true"))));
				extrusionLayer->setFillExtrusionColor(PropertyExpression<mbgl::Color>(interpolate(linear(),
					number(get("height")),
					0.f,
					toColor(literal("#160e23")),
					50.f,
					toColor(literal("#00615f")),
					100.f,
					toColor(literal("#55e9ff")))));
				extrusionLayer->setFillExtrusionOpacity(1.0f);
				extrusionLayer->setFillExtrusionHeight(PropertyExpression<float>(get("height")));
				extrusionLayer->setFillExtrusionBase(PropertyExpression<float>(get("min_height")));
				style.addLayer(std::move(extrusionLayer));
			}); 
	}
	void MapRenderCanvas::initCallback()
	{
		auto lambda_get_item_value_callback = [&](int itemId, std::string label)
			{
				mbgl::style::Layer* layer = m_Map->getStyle().getLayer(label);
				void* voidPtr = static_cast<void*>(layer); 
				if (layer)
				{
					return voidPtr;
				}
				else
				{
					return ((void*)0);
				}
			}; 
		auto lambda_toggle_componenents_callback = [&](const std::vector<std::pair<int, ItemData*>>& entities) {
			for (auto& [state, ItemData] : entities)
			{
				mbgl::style::Layer* ptr_layer = static_cast<mbgl::style::Layer*>(ItemData->GetData());
				if (ptr_layer)
				{
					ptr_layer->setVisibility(mbgl::style::VisibilityType(!state)); 
				}
			}
			};
		m_GetData_Item_Callback = std::make_shared<std::function<void* (int, std::string)>>(lambda_get_item_value_callback);
		m_Toggle_Components_Callback = std::make_shared<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>>(lambda_toggle_componenents_callback);
	}
	void MapRenderCanvas::bindFunction()
	{
		Bind(wxEVT_PAINT, &MapRenderCanvas::OnPaint, this);
		Bind(wxEVT_SIZE, &MapRenderCanvas::OnSize, this);
		Bind(wxEVT_MOTION, &MapRenderCanvas::OnMouseMove, this);
		Bind(wxEVT_MOUSEWHEEL, &MapRenderCanvas::OnMouseWheel, this);
		Bind(wxEVT_LEFT_DOWN, &MapRenderCanvas::OnMousePress, this);
		Bind(wxEVT_RIGHT_DOWN, &MapRenderCanvas::OnMousePress, this);
		Bind(wxEVT_LEFT_UP, &MapRenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_RIGHT_UP, &MapRenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_LEFT_DCLICK, &MapRenderCanvas::OnDoubleClick, this);
	}
	void MapRenderCanvas::OnSize(wxSizeEvent& event)
	{
		m_ContextLock->lock(); 
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		if (m_Backend) m_Backend->onWindowResize(viewPortSize.x, viewPortSize.y); 
		m_ContextLock->unlock(); 
		event.Skip();
	}
	void MapRenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		m_ContextLock->lock(); 
		if (m_Backend)m_Backend->runOnce();
		m_ContextLock->unlock(); 
		event.Skip(); 
	}
	void MapRenderCanvas::OnMouseMove(wxMouseEvent& event)
	{
		wxPoint pos = event.GetPosition();
		int x = pos.x; 
		int y = pos.y; 

		/*MOUSE STATE*/
		const wxSize size = this->getSize();
		m_MouseState.nor_mouse_pos.x = (pos.x / static_cast<float>(size.GetWidth())) * 2 - 1;
		m_MouseState.nor_mouse_pos.y = -(pos.y / static_cast<float>(size.GetHeight())) * 2 + 1;
		auto host = getCustomDrawableStyleLayerHost("Example-Bim-Layer");
		if (host)
		{
			host->mouseMove(m_MouseState.nor_mouse_pos); 
		}
		
		if (mode == MODE::VIEW)
		{

			if (m_Backend)
			{
				const double dx = x - m_Backend->m_lastX;
				const double dy = y - m_Backend->m_lastY;
				if (m_Backend->m_tracking)
				{
					if (dx || dy) {
						m_Backend->m_Map->moveBy(mbgl::ScreenCoordinate{ dx, dy });
					}
				}
				const int dx_p_r = std::abs(dx);
				const int dy_p_r = std::abs(dy);
				if (dx_p_r > dy_p_r)
				{
					if (m_Backend->m_rotating)
					{
						m_Backend->m_Map->rotateBy({ m_Backend->m_lastX, m_Backend->m_lastY }, { static_cast<double>(x), static_cast<double>(y) });
					}
				}
				else
				{
					if (m_Backend->m_pitching)
					{
						m_Backend->m_Map->pitchBy(dy / 2.0);
					}
				}

			}
		}
		if (m_Backend)
		{
			m_Backend->m_lastX = static_cast<double>(x);
			m_Backend->m_lastY = static_cast<double>(y);
		}
		event.Skip();
	}
	void MapRenderCanvas::OnMousePress(wxMouseEvent& event)
	{
		m_ContextLock->lock();
		int buttonFlag = event.GetButton();
		wxPoint pos = event.GetPosition();
		if (wxMOUSE_BTN_RIGHT == buttonFlag)
		{
			if (mode == MODE::VIEW)
			{
				if (m_Backend)
				{
					m_Backend->m_pitching = true;
					m_Backend->m_rotating = true;
				}
			}
		}
		else
		{
			if (buttonFlag == wxMOUSE_BTN_LEFT)
			{
				if (mode == MODE::VIEW)
				{
					if (m_Backend)
					{
						m_Backend->m_tracking = true;
					}
				}
				auto host = getCustomDrawableStyleLayerHost("Example-Bim-Layer");
				if (host)
				{
					host->query(m_MouseState.nor_mouse_pos);
				}
			}
		}
		m_ContextLock->unlock();
		event.Skip(); 
	}
	void MapRenderCanvas::OnMouseRelease(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		if (wxMOUSE_BTN_RIGHT == buttonFlag)
		{
			if (mode == MODE::VIEW)
			{
				m_Backend->m_pitching = false;
				m_Backend->m_rotating = false;
			}
		}
		else
		{
			if (buttonFlag == wxMOUSE_BTN_LEFT)
			{
				if (mode == MODE::VIEW)
				{
					if (m_Backend)
					{
						m_Backend->m_tracking = false;
					}
				}
				auto host = getCustomDrawableStyleLayerHost("Example-Bim-Layer");
				if (host)
				{
					host->mouseRelease(m_MouseState.nor_mouse_pos); 
				}
			}
		}
		event.Skip(); 
	}
	void MapRenderCanvas::OnMouseWheel(wxMouseEvent& event)
	{
		if (mode == MODE::VIEW)
		{
			int delta = event.GetWheelRotation();// 1 or -1
			double absDelta = delta < 0 ? -delta : delta;
			double scale = 2.0 / (1.0 + std::exp(-absDelta / 300.0));
			if (delta < 0 && scale != 0) {
				scale = 1.0 / scale;
			}
			scale = (scale - 1.0) / 2.0 + 1.0;
			if (m_Backend)
			{
				m_Backend->m_Map->scaleBy(scale, mbgl::ScreenCoordinate{ m_Backend->m_lastX, m_Backend->m_lastY },
					mbgl::AnimationOptions{ {mbgl::Milliseconds(100)} });
			}
		}
		event.Skip();
	}
	void MapRenderCanvas::OnDoubleClick(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		if (buttonFlag == wxMOUSE_BTN_LEFT)
		{
			if (m_Map && m_Backend)
			{
				m_Map->scaleBy(2.0,
					mbgl::ScreenCoordinate{ m_Backend->m_lastX, m_Backend->m_lastY },
					mbgl::AnimationOptions{ {mbgl::Milliseconds(500)} });
			}
		}
		event.Skip(); 
	}
	void MapRenderCanvas::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
	ThreeDCustomDrawableStyleLayerHost* MapRenderCanvas::getCustomDrawableStyleLayerHost(const std::string& name)
	{
		using namespace mbgl::style;
		using namespace mbgl::style::expression::dsl;
		MLN_TRACE_FUNC();
		mbgl::style::Style& style = m_Map->getStyle();
		const std::string identifier = name;
		mbgl::style::Layer* custom_layer = style.getLayer(identifier);
		if (!custom_layer) return nullptr; 
		CustomDrawableLayer* ptr_custom_drawable_layer = static_cast<CustomDrawableLayer*>(custom_layer);
		if (!ptr_custom_drawable_layer) return nullptr; 
		const mbgl::style::CustomDrawableLayer::Impl& custom_impl = ptr_custom_drawable_layer->impl();
		auto host = custom_impl.host;
		if (!host) return nullptr; 
		ThreeDCustomDrawableStyleLayerHost* ptr_host = static_cast<ThreeDCustomDrawableStyleLayerHost*>(host.get());
		if (!ptr_host) return nullptr; 
		return ptr_host; 
	}
	void MapRenderCanvas::activeContext()
	{
		SetCurrent(*m_Context); 
	}
	wxSize MapRenderCanvas::getSize()
	{
		return GetSize() * GetContentScaleFactor();
	}
	void MapRenderCanvas::Invalidate()
	{
	}
}