#pragma once
#ifndef _WIN_32_VIEW_
#define _WIN_32_VIEW_


#include <mbgl/map/map.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>



#include <utility>
#include <optional>

#include "Win32Backend.hpp"
#include "wxInclude.hpp"

namespace mbgl {
	namespace gfx {
		class RendererBackend;
	} // namespace gfx
} // namespace mbgl

namespace dragon
{
	class MapRenderCanvas;
}

namespace editor
{
	class Win32RenderFrontEnd; 
	class Win32View : public mbgl::MapObserver
	{
	public: 
		Win32View(dragon::MapRenderCanvas* canvas,
			bool fullscreen,
			bool benchmark,
			const mbgl::ResourceOptions& resourceOptions,
			const mbgl::ClientOptions& clientOptions);
		~Win32View() override; 
	public: 
		void invalidate(); 
		mbgl::gfx::RendererBackend& getRendererBackend();
		void setRenderFrontend(Win32RenderFrontEnd*);
		float getPixelRatio() const;
		mbgl::Size getSize() const;
		void setMap(mbgl::Map*);
		void runOnce(); 
		void render(); 
		void onWindowResize(int width, int height);
	public: 
		double m_lastX{ 0.0 };
		double m_lastY{ 0.0 };
		bool m_tracking = false;
		bool m_pitching = false;
		bool m_rotating = false;
		mbgl::Map* m_Map = nullptr;
	private: 

		std::unique_ptr<Win32Backend> m_Backend{ nullptr }; 
		Win32RenderFrontEnd* m_rendererFrontend = nullptr;


		dragon::MapRenderCanvas* m_canvas{ nullptr };
		bool m_Dirty = true;
		bool m_Fullscreen = false;
		bool m_Benchmark = false; 
		bool m_CapFrameRate{ false }; 
		int m_Width{ 0 }; 
		int m_Height{ 0 }; 
		float m_PixelRatio{ 0.0 }; 
		mbgl::util::RunLoop m_RunLoop;
		mbgl::util::Timer   m_FrameTick;

		mbgl::ResourceOptions m_mapResourceOptions;
		mbgl::ClientOptions m_mapClientOptions;
	};
}
#endif // !_WIN_32_VIEW_


