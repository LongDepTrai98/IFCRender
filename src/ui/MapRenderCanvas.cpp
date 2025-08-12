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
#include <nlohmann/json.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <fstream>
#include "core/lock/ContextLock.hpp"
#include "map/example_custom_drawable_style_layer.hpp"
namespace dragon
{
	MapRenderCanvas::MapRenderCanvas(wxWindow* parent,
		const wxGLAttributes& canvasAttrs) :
		IGLCanvas(parent,
			canvasAttrs)
	{
		ctxAttrs.PlatformDefaults()
			.CoreProfile()
			.OGLVersion(3, 0)
			.EndList();
		initGLContext(); 
		m_ContextLock->lock();
		bindFunction(); 
		initContextMap();
		initUI(); 
		m_ContextLock->unlock(); 
	}
	MapRenderCanvas::~MapRenderCanvas()
	{
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
		m_Map->getStyle().loadURL(orderedStyles[1].getUrl());
		m_Map->jumpTo(mbgl::CameraOptions()
			.withCenter(mbgl::LatLng{ 10.810507389340282, 106.66832163838852 })
			.withZoom(16)
			.withBearing(0.0)
			.withPitch(0.0));
	}
	void MapRenderCanvas::initUI()
	{
		int padding = 3;
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

		bimBtn->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
			{
				MLN_TRACE_FUNC();
				using namespace mbgl::style;
				using namespace mbgl::style::expression::dsl;
				mbgl::style::Style& style = m_Map->getStyle();
				const std::string identifier = "ExampleCustomDrawableStyleLayer";
				const auto& existingLayer = style.getLayer(identifier);

				if (!existingLayer) {
					style.addLayer(std::make_unique<mbgl::style::CustomDrawableLayer>(
						identifier, std::make_unique<ExampleCustomDrawableStyleLayerHost>("MLN_ASSETS_PATH")));
				}
				else {
					style.removeLayer(identifier);
				}
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
						layer->setVisibility(VisibilityType(false));
					}
					else
					{
						layer->setVisibility(VisibilityType(true));
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
				extrusionLayer->setFillExtrusionOpacity(0.5f);
				extrusionLayer->setFillExtrusionHeight(PropertyExpression<float>(get("height")));
				extrusionLayer->setFillExtrusionBase(PropertyExpression<float>(get("min_height")));
				style.addLayer(std::move(extrusionLayer));
			}); 
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
			m_Backend->m_lastX = static_cast<double>(x);
			m_Backend->m_lastY = static_cast<double>(y);
			
		}
		event.Skip();
	}
	void MapRenderCanvas::OnMousePress(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		wxPoint pos = event.GetPosition();
		if (wxMOUSE_BTN_RIGHT == buttonFlag)
		{
			if (m_Backend)
			{
				m_BeginX = pos.x;
				m_BeginY = pos.y;
				m_Backend->m_pitching = true; 
				m_Backend->m_rotating = true; 
			}
		}
		else
		{
			if (buttonFlag == wxMOUSE_BTN_LEFT)
			{
				if (m_Backend)
				{
					m_Backend->m_tracking = true; 
				}
			}
		}
		event.Skip(); 
	}
	void MapRenderCanvas::OnMouseRelease(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		if (wxMOUSE_BTN_RIGHT == buttonFlag)
		{
			m_Backend->m_pitching = false; 
			m_Backend->m_rotating = false; 
		}
		else
		{
			if (buttonFlag == wxMOUSE_BTN_LEFT)
			{
				if (m_Backend)
				{
					m_Backend->m_tracking = false;
				}
			}
		}
		event.Skip(); 
	}
	void MapRenderCanvas::OnMouseWheel(wxMouseEvent& event)
	{
		int delta = event.GetWheelRotation();// 1 or -1
		double absDelta = delta < 0 ? -delta : delta;
		double scale = 2.0 / (1.0 + std::exp(-absDelta / 100.0));
		if (delta < 0 && scale != 0) {
			scale = 1.0 / scale;
		}
		scale = (scale - 1.0) / 2.0 + 1.0;
		if (m_Backend)
		{
			m_Backend->m_Map->scaleBy(scale, mbgl::ScreenCoordinate{ m_Backend->m_lastX, m_Backend->m_lastY },
				mbgl::AnimationOptions{ {mbgl::Milliseconds(100)} });
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