#ifndef _VIEW_PORT_GIZMO_HPP_
#define _VIEW_PORT_GIZMO_HPP_
#include "core/IViewPort.hpp"
#include "core/IRenderer.hpp"
namespace dragon
{
	/*SCENE FOR GIZMO*/
	class ViewPortGizmo : public ViewPort,
		public IRenderer
	{
	public:
		ViewPortGizmo(RenderCanvas* canvas);
		~ViewPortGizmo();
	public:
		void initCamera(threepp::WindowSize& w_size) override;
		void initScene(threepp::WindowSize& w_size) override;
	public:
		void resize(const int& width, const int& height) override;
		void update(const float& dtTime) override;
		void render(threepp::GLRenderer* renderer) override;
		void handleRaycast(const double& norX, const double& norY) override; 
	private: 
		std::shared_ptr<threepp::Group> createAxisArrow(); 
	protected:
		int m_Padding{ 0 };
		float m_AxisLength{ 2.5f }; 
		float m_HeadLength{0.3f}; 
		float m_HeadWidth{0.4f}; 
	};
}
#endif // !_VIEW_PORT_GIZMO_HPP_