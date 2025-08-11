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
#include <fstream>
namespace dragon
{
	MapRenderCanvas::MapRenderCanvas(wxWindow* parent/*, const wxGLAttributes& canvasAttrs,*/, wxGLContext* context) :
		wxGLCanvas(parent), 
		m_Context(context)
	{
		//initGLContext(); 
		//SetCurrent(*m_Context); 
		initContextMap(); 
		bindFunction(); 
	}
	MapRenderCanvas::~MapRenderCanvas()
	{
	}
	void MapRenderCanvas::initGLContext()
	{
	/*	wxGLContextAttrs ctxAttrs;
		ctxAttrs.PlatformDefaults()
			.CoreProfile()
			.OGLVersion(3, 0)
			.EndList();
		if (!m_Context)
			m_Context = std::make_unique<wxGLContext>(this, nullptr, &ctxAttrs);
		if (!m_Context->IsOK())
		{
			throw std::exception("Can't create context renderer");
		}*/
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
		m_Map->getStyle().loadURL(orderedStyles[0].getUrl());

		m_Map->jumpTo(mbgl::CameraOptions()
			.withCenter(mbgl::LatLng{ 0.0, 0.0 })
			.withZoom(0.0)
			.withBearing(0.0)
			.withPitch(0.0));
	}
	void MapRenderCanvas::bindFunction()
	{
		Bind(wxEVT_PAINT, &MapRenderCanvas::OnPaint, this);
		Bind(wxEVT_SIZE, &MapRenderCanvas::OnSize, this);
		Bind(wxEVT_MOTION, &MapRenderCanvas::OnMouseMove, this);
		Bind(wxEVT_MOUSEWHEEL, &MapRenderCanvas::OnMouseWheel, this);
	}
	void MapRenderCanvas::OnSize(wxSizeEvent& event)
	{
		SetCurrent(*m_Context);
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		if (m_Backend) m_Backend->onWindowResize(viewPortSize.x, viewPortSize.y); 
		wglMakeCurrent(NULL, NULL);
		event.Skip();
	}
	void MapRenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		//SetCurrent(*m_Context);
		if (m_Backend)m_Backend->runOnce();
		//wglMakeCurrent(NULL, NULL); 
	}
	void MapRenderCanvas::OnMouseMove(wxMouseEvent& event)
	{
		wxPoint pos = event.GetPosition();
		if (m_Backend)m_Backend->m_lastX = static_cast<double>(pos.x);
		if (m_Backend)m_Backend->m_lastY = static_cast<double>(pos.y);
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
	void MapRenderCanvas::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
	void MapRenderCanvas::activeContext()
	{
		SetCurrent(*m_Context); 
	}
}