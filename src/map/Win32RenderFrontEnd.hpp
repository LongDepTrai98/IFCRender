#pragma once
#ifndef _WIN_32_RENDER_FRONT_END_
#define _WIN_32_RENDER_FRONT_END_
#include <mbgl/renderer/renderer_frontend.hpp>
namespace mbgl {
	class Renderer;
} // namespace mbgl
namespace editor
{
	class Win32View; 
	class Win32RenderFrontEnd : public mbgl::RendererFrontend 
	{
	public: 
		Win32RenderFrontEnd(std::unique_ptr<mbgl::Renderer>, Win32View&);
		~Win32RenderFrontEnd() override;
		void reset() override;
		void setObserver(mbgl::RendererObserver&) override;
		void update(std::shared_ptr<mbgl::UpdateParameters>) override;
		const mbgl::TaggedScheduler& getThreadPool() const override;
		void render();
		mbgl::Renderer* getRenderer();

	private: 
		Win32View& m_Win32View;
		std::unique_ptr<mbgl::Renderer> m_Renderer;
		std::shared_ptr<mbgl::UpdateParameters> m_UpdateParameters;
	};
}
#endif // !_WIN_32_RENDER_FRONT_END_


