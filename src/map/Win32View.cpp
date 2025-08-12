#include "Win32View.hpp"
#include "core/utils/AppHelper.hpp"
#include "Win32RenderFrontEnd.hpp"
#include <mbgl/annotation/annotation.hpp>
#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/math/angles.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>
#include <mapbox/geometry/point.hpp>
#include "ui/MapRenderCanvas.hpp"
namespace editor
{
	Win32View::Win32View(dragon::MapRenderCanvas* canvas,
		bool fullscreen,
		bool benchmark,
		const mbgl::ResourceOptions& resourceOptions,
		const mbgl::ClientOptions& clientOptions) : m_canvas(canvas)
		,m_Fullscreen(fullscreen),
		m_Benchmark(benchmark),
		m_mapResourceOptions(resourceOptions.clone()),
		m_mapClientOptions(clientOptions.clone())
	{
		//glfwSwapInterval(1); vsync
		dragon::AppHelper::GetWindowSize(m_canvas,
			m_Width,
			m_Height);
		m_Backend = Win32Backend::Create(m_canvas, m_CapFrameRate);
		m_PixelRatio = static_cast<float>(m_Backend->getSize().width) / m_Width;
		//wglMakeCurrent(nullptr, nullptr); 

	}
	Win32View::~Win32View()
	{
		MLN_TRACE_FUNC();

	}
	void Win32View::invalidate()
	{
		MLN_TRACE_FUNC();
		m_Dirty = true;
	}
	mbgl::gfx::RendererBackend& Win32View::getRendererBackend()
	{
		MLN_TRACE_FUNC();

		return m_Backend->getRendererBackend();
	}
	void Win32View::setRenderFrontend(Win32RenderFrontEnd* rendererFrontend_)
	{
		MLN_TRACE_FUNC();
		m_rendererFrontend = rendererFrontend_;
	}
	float Win32View::getPixelRatio() const
	{
		return m_PixelRatio; 
	}
	mbgl::Size Win32View::getSize() const
	{
		return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) };
	}
	void Win32View::setMap(mbgl::Map* map_)
	{
		MLN_TRACE_FUNC();
		m_Map = map_; 
	}
	void Win32View::runOnce()
	{
		MLN_TRACE_FUNC();
		render();
		m_RunLoop.runOnce();
	}
	void Win32View::render()
	{
		if (m_Dirty && m_rendererFrontend) {
			m_Dirty = false;
			mbgl::gfx::BackendScope scope{ m_Backend->getRendererBackend() };
			m_rendererFrontend->render();
		}
	}
	void Win32View::onWindowResize(int width, int height)
	{
		MLN_TRACE_FUNC();
		if (width == 0 || height == 0)
			return; 
		m_Width = width; 
		m_Height = height;
		m_Backend->setSize({ static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }); 
		m_Map->setSize({ static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) });
		invalidate(); 
	}
}
