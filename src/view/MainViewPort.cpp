#include "MainViewPort.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"
namespace dragon
{
	MainViewPort::MainViewPort(RenderCanvas* canvas) : IRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Viewport_Size = { canvas_size.x, canvas_size.y }; 
		initScene(m_Viewport_Size);
		initCamera(m_Viewport_Size);
	}
	MainViewPort::~MainViewPort()
	{
	}
	void MainViewPort::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(60, w_size.aspect()); 
		m_Camera->position.z = 10;
	}
	void MainViewPort::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>(); 
		m_Scene->background = threepp::Color::whitesmoke; 
	}
	void MainViewPort::resize(const int& width, const int& height)
	{
		m_Viewport_Size = { width,height }; 
		if (m_Camera)
		{
			m_Camera->aspect = m_Viewport_Size.aspect(); 
			m_Camera->updateProjectionMatrix(); 
		}
	}
	void MainViewPort::update(const float& dtTime)
	{
	}
	void MainViewPort::render(threepp::GLRenderer* renderer)
	{
		if (m_Renderer
			&& m_Scene
			&& m_Camera)
		{
			renderer->setViewport({0,0,m_Viewport_Size.width(),m_Viewport_Size.height()});
			renderer->render(*m_Scene.get(), *m_Camera.get()); 
		}
	}
}