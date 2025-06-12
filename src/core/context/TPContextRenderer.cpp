#include "TPContextRenderer.hpp"
#include "ui/SceneView.hpp"
/*
* THREEPP CONTEXT RENDERER
* BACKEND RENDERER IS OPENGL + THREEPP 
*/
namespace dragon
{
	TPContextRenderer::TPContextRenderer(SceneView* canvas) : IContextRenderer(canvas)
	{
	}
	void TPContextRenderer::resize(const int& width, const int& height)
	{
		/*
		* Context render resize 
		*/
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
			glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Canvas->swapBuff(); 
			m_Canvas->deactiveContext(); 
		}
	}
	TPContextRenderer::~TPContextRenderer()
	{
	}
}