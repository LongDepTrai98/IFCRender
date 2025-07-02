#include "ViewportGizmo.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
namespace dragon
{
	namespace example
	{
		using namespace threepp;
		auto createAxes() {
			const auto axes = AxesHelper::create(2.0f);
			return axes; 
		}
	}
	ViewPortGizmo::ViewPortGizmo(RenderCanvas* canvas) : IRenderer(canvas)
	{
		m_Canvas->activeContext();
		m_Viewport_Size = { 100,100 }; 
		wxSize canvas_size = canvas->getSize();
		initCamera(m_Viewport_Size);
		initScene(m_Viewport_Size);
		m_Camera->position.z = 5;
		example::createAxes();
		m_Scene->add(createAxisArrow());
		m_Canvas->deactiveContext();
	}
	ViewPortGizmo::~ViewPortGizmo()
	{
	}
	void ViewPortGizmo::resize(const int& width, const int& height)
	{
		if (m_Camera)
		{
			m_Camera->aspect = m_Viewport_Size.aspect();
			m_Camera->updateProjectionMatrix();
		}
	}
	void ViewPortGizmo::update(const float& dtTime)
	{
	}
	void ViewPortGizmo::render(threepp::GLRenderer* renderer)
	{
		if (renderer
			&& m_Scene
			&& m_Camera)
		{
			glDisable(GL_DEPTH_TEST); 
			renderer->setViewport({ m_Padding, m_Padding, m_Viewport_Size.width(), m_Viewport_Size.height() });
			renderer->render(*m_Scene, *m_Camera);
			glEnable(GL_DEPTH_TEST); 
		}
	}
	void ViewPortGizmo::handleRaycast(threepp::Vector2& nor_mouse_pos)
	{
		/*HANDLE RAY CASTER*/
	}
	void ViewPortGizmo::clearScene()
	{
	}
	std::shared_ptr<threepp::Group> ViewPortGizmo::createAxisArrow()
	{
		std::shared_ptr<threepp::Group> axisGroup = threepp::Group::create(); 
		auto xAxis = threepp::ArrowHelper::create({ 1, 0, 0 }, { 0, 0, 0 }, m_AxisLength, 0xff0000,m_HeadLength,m_HeadWidth);
		auto yAxis = threepp::ArrowHelper::create({ 0, 1, 0 }, { 0, 0, 0 }, m_AxisLength, 0x00ff00,m_HeadLength,m_HeadWidth);
		auto zAxis = threepp::ArrowHelper::create({ 0, 0, 1 }, { 0, 0, 0 }, m_AxisLength, 0x0000ff,m_HeadLength,m_HeadWidth);
		axisGroup->add(xAxis); 
		axisGroup->add(yAxis); 
		axisGroup->add(zAxis); 
		return axisGroup;
	}
	void ViewPortGizmo::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(60, w_size.aspect());
	}
	void ViewPortGizmo::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>();
	}
}