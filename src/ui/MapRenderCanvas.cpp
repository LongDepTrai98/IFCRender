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
#include "core/lock/ContextLock.hpp"
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
			/*const int dx = std::abs(m_BeginX - x);
			const int dy = std::abs(m_BeginY - y);*/
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