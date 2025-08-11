#include "Win32RenderFrontEnd.hpp"
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/util/instrumentation.hpp>
#include "Win32View.hpp"
namespace editor
{
	Win32RenderFrontEnd::Win32RenderFrontEnd(std::unique_ptr<mbgl::Renderer> renderer_,
		Win32View& win32View) : m_Win32View(win32View),
		m_Renderer(std::move(renderer_))
	{
		m_Win32View.setRenderFrontend(this); 
	}

	Win32RenderFrontEnd::~Win32RenderFrontEnd() = default;

	void Win32RenderFrontEnd::reset()
	{
		assert(m_Renderer);
		m_Renderer.reset();
	}

	void Win32RenderFrontEnd::setObserver(mbgl::RendererObserver& observer)
	{
		assert(m_Renderer);
		m_Renderer->setObserver(&observer);
	}

	void Win32RenderFrontEnd::update(std::shared_ptr<mbgl::UpdateParameters> params)
	{
		m_UpdateParameters = std::move(params);
		m_Win32View.invalidate();
	}

	const mbgl::TaggedScheduler& Win32RenderFrontEnd::getThreadPool() const
	{
		// TODO: insert return statement here
		return m_Win32View.getRendererBackend().getThreadPool();

	}

	void Win32RenderFrontEnd::render()
	{
		MLN_TRACE_FUNC();

		assert(m_Renderer);

		if (!m_UpdateParameters) return;

		mbgl::gfx::BackendScope guard{ m_Win32View.getRendererBackend(), mbgl::gfx::BackendScope::ScopeType::Implicit };

		// onStyleImageMissing might be called during a render. The user implemented
		// method could trigger a call to MLNRenderFrontend#update which overwrites
		// `updateParameters`. Copy the shared pointer here so that the parameters
		// aren't destroyed while `render(...)` is still using them.
		auto updateParameters_ = m_UpdateParameters;
		m_Renderer->render(updateParameters_);
	}

	mbgl::Renderer* Win32RenderFrontEnd::getRenderer()
	{
		assert(m_Renderer);
		return m_Renderer.get();
	}

}

