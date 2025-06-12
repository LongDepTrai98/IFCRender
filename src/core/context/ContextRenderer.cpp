#include "ContextRenderer.hpp"
#include "ui/SceneView.hpp"
namespace dragon
{
	ContextRenderer::ContextRenderer(SceneView* canvas) : m_Canvas(canvas)
	{
	}
	void ContextRenderer::resize(const int& width, const int& height)
	{
		/*
		* Context render resize 
		*/
	}
	void ContextRenderer::update(const float& dtTime)
	{
	}
	void ContextRenderer::render()
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
	ContextRenderer::~ContextRenderer()
	{
	}
}