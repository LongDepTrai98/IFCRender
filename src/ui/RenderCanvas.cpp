#include "RenderCanvas.hpp"
#include <iostream>
#include "renderer/THREEPPRenderer.hpp"
namespace dragon
{
	RenderCanvas::RenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs) : wxGLCanvas(parent,
				canvasAttrs)
	{
		bindFunction(); 
		initGLContext(); 
		initContextRenderer(); 
	}
	RenderCanvas::~RenderCanvas()
	{
	}
	void RenderCanvas::initGLContext()
	{
		wxGLContextAttrs ctxAttrs;
		ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
		if (!m_Context)
			m_Context = std::make_unique<wxGLContext>(this, nullptr, &ctxAttrs);
		if (!m_Context->IsOK())
		{
			throw std::exception("Can't create context renderer");
		}
	}
	void RenderCanvas::initContextRenderer()
	{
		if (!m_Renderer)
			m_Renderer = std::make_unique<THREEPPRenderer>(this);
	}
	void RenderCanvas::bindFunction()
	{
		Bind(wxEVT_PAINT, &RenderCanvas::OnPaint, this);
		Bind(wxEVT_SIZE, &RenderCanvas::OnSize, this);
		Bind(wxEVT_MOTION, &RenderCanvas::OnMouseMove, this);
		Bind(wxEVT_LEFT_DOWN, &RenderCanvas::OnMousePress, this);
		Bind(wxEVT_RIGHT_DOWN, &RenderCanvas::OnMousePress, this);
		Bind(wxEVT_LEFT_UP, &RenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_RIGHT_UP, &RenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_MOUSEWHEEL, &RenderCanvas::OnMouseWheel, this);
	}
	void RenderCanvas::activeContext()
	{
		SetCurrent(*m_Context);
	}
	void RenderCanvas::deactiveContext()
	{
		wglMakeCurrent(NULL, NULL);
	}
	void RenderCanvas::swapBuff()
	{
		SwapBuffers(); 
	}
	wxSize RenderCanvas::getSize()
	{
		return GetSize() * GetContentScaleFactor();
	}
	void RenderCanvas::OnSize(wxSizeEvent& event)
	{
		activeContext();
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		if (m_Renderer)
		{
			m_Renderer->resize(viewPortSize.x,
				viewPortSize.y); 
		}
		deactiveContext(); 
		event.Skip(); 
	}
	void RenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		activeContext(); 
		if (m_Renderer)
		{
			m_Renderer->update(m_dtTime);
			m_Renderer->render();
		}
		swapBuff(); 
		deactiveContext(); 
	}
	void RenderCanvas::OnMouseMove(wxMouseEvent& event)
	{
	}
	void RenderCanvas::OnMousePress(wxMouseEvent& event)
	{
	}
	void RenderCanvas::OnMouseRelease(wxMouseEvent& event)
	{
	}
	void RenderCanvas::OnMouseWheel(wxMouseEvent& event)
	{
	}
	void RenderCanvas::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
}