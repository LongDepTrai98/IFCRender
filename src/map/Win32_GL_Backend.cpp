#include "Win32_GL_Backend.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include <mbgl/util/instrumentation.hpp>
#include "core/utils/AppHelper.hpp"
#include "ui/MapRenderCanvas.hpp"
#include "core/lock/ContextLock.hpp"
namespace editor
{
	class Win32GLRenderableResource final : public mbgl::gl::RenderableResource {
	public:
		explicit Win32GLRenderableResource(Win32_GL_Backend& backend_)
			: backend(backend_) {
		}

		void bind() override {
			MLN_TRACE_FUNC();

			backend.setFramebufferBinding(0);
			backend.setViewport(0, 0, backend.getSize());
		}

		void swap() override {
			MLN_TRACE_FUNC();

			backend.swap();
		}

	private:
		Win32_GL_Backend& backend;
	};


	Win32_GL_Backend::Win32_GL_Backend(dragon::MapRenderCanvas* canvas_, bool capFrameRate) :
		mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
		mbgl::gfx::Renderable(
			[canvas_] {
				int fbWidth;
				int fbHeight;
				dragon::AppHelper::GetFramebufferSize(canvas_,
					&fbWidth,
					&fbHeight);
				return mbgl::Size{ static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight) };
			}(),
				std::make_unique<Win32GLRenderableResource>(*this)),
		m_canvas(canvas_)
	{
		m_canvas->getContextLock()->lock(); 
	}
	Win32_GL_Backend::~Win32_GL_Backend() = default; 

	void Win32_GL_Backend::swap()
	{
		m_canvas->SwapBuffers(); 
	}
	mbgl::Size Win32_GL_Backend::getSize() const
	{
		return size;
	}
	void Win32_GL_Backend::setSize(const mbgl::Size newSize)
	{
		size = newSize;
	}
	void Win32_GL_Backend::activate()
	{
		MLN_TRACE_FUNC();
		m_canvas->getContextLock()->lock(); 

	}
	void Win32_GL_Backend::deactivate()
	{
		MLN_TRACE_FUNC();
		m_canvas->getContextLock()->unlock(); 
	}
	mbgl::gl::ProcAddress Win32_GL_Backend::getExtensionFunctionPointer(const char* name)
	{
		return reinterpret_cast<void(*)()>(wglGetProcAddress(name));
	}
	void Win32_GL_Backend::updateAssumedState()
	{
		MLN_TRACE_FUNC();
		assumeFramebufferBinding(0);
		setViewport(0, 0, size);
	}
}

namespace mbgl {
	namespace gfx {

		template <>
		std::unique_ptr<editor::Win32Backend> Backend::Create<mbgl::gfx::Backend::Type::OpenGL>(dragon::MapRenderCanvas* window, bool capFrameRate) {
			MLN_TRACE_FUNC();

			return std::make_unique<editor::Win32_GL_Backend>(window, capFrameRate);
		}

	} // namespace gfx
} // namespace mbgl
