#ifndef _CONTEXT_RENDERER_HPP_
#define _CONTEXT_RENDERER_HPP_
#include "core/IRenderer.hpp"
#include "core/IWindowEventHandler.hpp"
#include "SceneContext.hpp"
namespace dragon
{
	/*
	* THREEPP BACKEND RENDERER
	*/
	class THREEPPRenderer : public IRenderer,
		public SceneContext,
		public WindowEventHandler,
		public threepp::PeripheralsEventSource
	{
	public:
		THREEPPRenderer(RenderCanvas* canvas);
		virtual ~THREEPPRenderer();
	public:
		threepp::GLRenderer* getRenderer();
	private:
		void initRenderer(threepp::WindowSize& w_size);
		void initCamera(threepp::WindowSize& w_size);
		void initScene(threepp::WindowSize& w_size);
		void validateContext();
		void initController();
		void ctxRender();
		//example
	private:
		void createExampleScene();
	public:
		virtual void resize(const int& width, const int& height) override;
		virtual void update(const float& dtTime) override;
		virtual void render() override;
		//event
		void OnMouseMove(wxMouseEvent& event) override;
		void OnMousePress(wxMouseEvent& event) override;
		void OnMouseRelease(wxMouseEvent& event) override;
		void OnMouseWheel(wxMouseEvent& event) override;
		//threepp
		threepp::WindowSize size() const override;
	protected:
		//Renderer
		std::unique_ptr<threepp::GLRenderer> m_Renderer{ nullptr };
		/*CONTROLLER*/
		std::unique_ptr<threepp::OrbitControls> m_OrbitControls{ nullptr };
	};
}
#endif // !_CONTEXT_RENDERER_HPP_