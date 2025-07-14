#ifndef _CONTEXT_RENDERER_HPP_
#define _CONTEXT_RENDERER_HPP_
#include "core/IRenderer.hpp"
#include "core/IWindowEventHandler.hpp"
#include "threepp/threepp.hpp"
namespace threepp
{
	class Raycaster;
}
namespace dragon
{
	/*
	* THREEPP BACKEND RENDERER
	*/
	class ViewPort;
	class THREEPPRenderer : public IRenderer,
		public WindowEventHandler,
		public threepp::PeripheralsEventSource
	{
	public:
		THREEPPRenderer(RenderCanvas* canvas);
		virtual ~THREEPPRenderer();
	public:
		threepp::GLRenderer* getRenderer();
		ViewPort* getMainViewPort();
	private:
		void initRenderer(threepp::WindowSize& w_size);
		void initViewPort();
		//void initCamera(threepp::WindowSize& w_size);
		//void initScene(threepp::WindowSize& w_size);
		void validateContext();
		void initController();
		void ctxRender();
		//example
	private:
	public:
		void render();
		void resize(const int& width, const int& height);
		virtual void update(const float& dtTime) override;
		//event
		void OnMouseMove(wxMouseEvent& event) override;
		void OnMousePress(wxMouseEvent& event) override;
		void OnMouseRelease(wxMouseEvent& event) override;
		void OnMouseWheel(wxMouseEvent& event) override;
		void UpdateGizmoFromCamera();
		//threepp
		threepp::WindowSize size() const override;
	protected:
		std::unique_ptr<threepp::GLRenderer> m_Renderer{ nullptr };
		/*CONTROLLER*/
		std::unique_ptr<threepp::OrbitControls> m_OrbitControls{ nullptr };
		/*LST VIEWPORT*/
		std::vector<std::unique_ptr<ViewPort>> m_lstViewPort{};
		/*RAY CASTER*/
		threepp::Vector2 nor_mouse_pos{ -threepp::Infinity<float>, -threepp::Infinity<float> };
	};
}
#endif // !_CONTEXT_RENDERER_HPP_