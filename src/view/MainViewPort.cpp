#include "MainViewPort.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/helpers/SpotLightHelper.hpp"

namespace dragon
{
	static void createAndAddLights(threepp::Scene& scene) {

		auto light = threepp::SpotLight::create(0xffffff, 0.5f);
		scene.add(threepp::AmbientLight::create(threepp::Color::lightgray, .5f));
		light->position.set(100, 100, 0);
		light->angle = threepp::math::PI / 9;
		light->castShadow = true;
		scene.add(light); 
		auto helper = threepp::SpotLightHelper::create(*light);
		scene.add(helper);
		const auto axes = threepp::AxesHelper::create(100);
		scene.add(axes);
	}
	MainViewPort::MainViewPort(RenderCanvas* canvas) : IRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Viewport_Size = { canvas_size.x, canvas_size.y }; 
		initScene(m_Viewport_Size);
		initCamera(m_Viewport_Size);
		createAndAddLights(*m_Scene); 
		createExampleScene(); 
	
	}
	MainViewPort::~MainViewPort()
	{
	}
	void MainViewPort::createExampleScene()
	{
		m_Camera->position.z = 5;
	}
	void MainViewPort::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(75, w_size.aspect()); 
	}
	void MainViewPort::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>(); 
		m_Scene->background = threepp::Color::aliceblue; 
	}
	void MainViewPort::addLight()
	{
		createAndAddLights(*m_Scene);
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