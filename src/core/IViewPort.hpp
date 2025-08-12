#ifndef _SCENE_CONTEXT_THREEPP_
#define _SCENE_CONTEXT_THREEPP_
#include "threepp/threepp.hpp"
#include "threepp/math/Vector2.hpp"
#include "core/io/IGeometryCache.hpp"
#include <memory>
class wxKeyEvent;
namespace threepp
{
	class Raycaster;
	class GLRenderTarget;
	class Material;
}
namespace dragon
{
	/*INTERFACE VIEWPORT*/
	class MouseState;
	class KeyData;
	class EventData;
	class ToolBarData;
	class MenuData;
	class ViewPort
	{
	public:
		virtual ~ViewPort() = default;
	public:
		threepp::Camera* getCamera()
		{
			return m_Camera.get();
		};
		void initRenderer(threepp::GLRenderer* renderer)
		{
			m_Renderer = renderer;
		};
	public:
		virtual threepp::Scene* getScene() = 0;
		virtual void initCamera(threepp::WindowSize& w_size) = 0;
		virtual void initScene(threepp::WindowSize& w_size) = 0;
		virtual void render(threepp::GLRenderer* main_render) = 0;
		virtual void resize(const int& width, const int& height) = 0;
		virtual void handleRaycast(MouseState& mouse_state) = 0;
		virtual void OnLButtonDown(EventData& data) = 0;
		virtual void OnLButtonUp(EventData& data) = 0;
		virtual void OnRButtonDown(EventData& data) = 0;
		virtual void OnRButtonUp(EventData& data) = 0;
		virtual void clearScene() = 0;
		virtual void OnKeyDown(KeyData& event) = 0;
		virtual void OnKeyUp(KeyData& event) = 0;
		virtual void OnToolBarAction(ToolBarData& data) = 0;
		virtual void OnMenuClick(MenuData& data) = 0;
	public:
		/*TOOL ACTIONS*/
		virtual void OnToolActions(int toolID) = 0;
		//virtual void
	public:
		std::function<void(const std::vector<std::shared_ptr<threepp::Mesh>>& meshes)> m_Add_Object_CallBack{ nullptr };
		std::function<void(const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries, bool)> m_Add_Object_DrawDepth_CallBack{ nullptr };
	protected:
		/*SCENE CONTEXT*/
		//std::unique_ptr<threepp::Scene> m_Scene{ nullptr };
		threepp::Scene* m_Scene{ nullptr };
		std::unique_ptr<threepp::PerspectiveCamera> m_Camera{ nullptr };
		threepp::GLRenderer* m_Renderer{ nullptr };
		threepp::WindowSize m_Viewport_Size{};
	};
}
#endif // !_SCENE_CONTEXT_THREEPP_