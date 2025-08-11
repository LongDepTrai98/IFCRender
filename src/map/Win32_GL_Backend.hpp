#pragma once
#ifndef _WIN_32_GL_BACKEND_HPP_
#define _WIN_32_GL_BACKEND_HPP_
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>
#include "Win32Backend.hpp"
#include "wxInclude.hpp"
class WindowViewMap; 
namespace dragon
{
	class MapRenderCanvas; 
}
namespace editor
{

	class Win32_GL_Backend : public Win32Backend, 
		public mbgl::gl::RendererBackend,
		public mbgl::gfx::Renderable
	{
	public: 
		Win32_GL_Backend(dragon::MapRenderCanvas*, bool capFrameRate);
		~Win32_GL_Backend() override;

		void swap();

	public:
		mbgl::gfx::RendererBackend& getRendererBackend() override { return *this; }
		mbgl::Size getSize() const override;
		void setSize(mbgl::Size) override;

		// mbgl::gfx::RendererBackend implementation
	public:
		mbgl::gfx::Renderable& getDefaultRenderable() override { return *this; }

	protected:
		void activate() override;
		void deactivate() override;

		// mbgl::gl::RendererBackend implementation
	protected:
		mbgl::gl::ProcAddress getExtensionFunctionPointer(const char*) override;
		void updateAssumedState() override;

	private: 
		dragon::MapRenderCanvas* m_canvas{ nullptr };
	};
}
#endif // !_WIN_32_GL_BACKEND_HPP_


