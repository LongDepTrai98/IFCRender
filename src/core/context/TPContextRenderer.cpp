#include "TPContextRenderer.hpp"
#include "ui/SceneView.hpp"
#include "threepp/threepp.hpp"
/*
* THREEPP CONTEXT RENDERER
* BACKEND RENDERER IS OPENGL + THREEPP 
*/
namespace dragon
{
	TPContextRenderer::TPContextRenderer(SceneView* canvas) : IContextRenderer(canvas)
	{
		m_Canvas->activeContext(); 
		wxSize canvas_size = m_Canvas->getSize(); 
		//create windowsize 
		threepp::WindowSize window_size(canvas_size.x, canvas_size.y);
		initRenderer(window_size);
		initScene(window_size);
		initCamera(window_size); 
		m_Canvas->deactiveContext(); 
	}
	TPContextRenderer::~TPContextRenderer()
	{
	}
	void TPContextRenderer::initRenderer(threepp::WindowSize& w_size)
	{
		if (!m_Renderer)
			m_Renderer = std::make_unique<threepp::GLRenderer>(w_size);
	}
	void TPContextRenderer::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(75, w_size.aspect());
	}
	void TPContextRenderer::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>(); 
		m_Scene->background = threepp::Color::aliceblue; 
	}
	void TPContextRenderer::ctxRender()
	{
		if (m_Renderer
			&& m_Scene
			&& m_Camera)
		{
			m_Renderer->render(*m_Scene, *m_Camera); 
		}
	}
	void TPContextRenderer::resize(const int& width, const int& height)
	{
		/*
		* Context render resize 
		*/
		threepp::WindowSize window_size(width,height);
		if (m_Camera)
		{
			m_Camera->aspect = window_size.aspect(); 
			m_Camera->updateProjectionMatrix(); 
			m_Renderer->setSize(window_size); 
		}
	}
	void TPContextRenderer::update(const float& dtTime)
	{
	}
	void TPContextRenderer::render()
	{
		if (m_Canvas)
		{
			m_Canvas->activeContext();
			/*
			* context render here 
			*/
			ctxRender();
			m_Canvas->swapBuff(); 
			m_Canvas->deactiveContext(); 
		}
	}
}