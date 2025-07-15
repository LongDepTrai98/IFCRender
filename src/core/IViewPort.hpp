#ifndef _SCENE_CONTEXT_THREEPP_
#define _SCENE_CONTEXT_THREEPP_
#include "threepp/threepp.hpp"
#include "threepp/math/Vector2.hpp"
#include "core/io/IGeometryCache.hpp"
#include <memory>
namespace threepp
{
	class Raycaster;
}
namespace dragon
{
	/*INTERFACE VIEWPORT*/
	class MouseState; 
	class ViewPort
	{
	public:
		virtual ~ViewPort() = default;
	public:
		threepp::Scene* getScene()
		{
			return m_Scene.get();
		};
		threepp::Camera* getCamera()
		{
			return m_Camera.get();
		};
		void initRenderer(threepp::GLRenderer* renderer)
		{
			m_Renderer = renderer;
		};
	public:
		virtual void initCamera(threepp::WindowSize& w_size) = 0;
		virtual void initScene(threepp::WindowSize& w_size) = 0;
		virtual void render(threepp::GLRenderer* main_render) = 0;
		virtual void resize(const int& width, const int& height) = 0;
		virtual void handleRaycast(MouseState& mouse_state) = 0;
		virtual void OnLButtonDown() = 0;
		virtual void OnLButtonUp() = 0;
		virtual void OnRButtonDown() = 0;
		virtual void OnRButtonUp() = 0; 
		virtual void clearScene() = 0; 
		//virtual void
	protected:
		/*SCENE CONTEXT*/
		std::unique_ptr<threepp::Scene> m_Scene{ nullptr };
		std::unique_ptr<threepp::PerspectiveCamera> m_Camera{ nullptr };
		threepp::GLRenderer* m_Renderer{ nullptr };
		threepp::WindowSize m_Viewport_Size{};
	};
}
#endif // !_SCENE_CONTEXT_THREEPP_