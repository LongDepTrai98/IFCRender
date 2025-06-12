#include "RenderCanvas.hpp"
#include <iostream>
#include "core/context/TPContextRenderer.hpp"
namespace dragon
{
	wxBEGIN_EVENT_TABLE(RenderCanvas, wxGLCanvas)
		EVT_SIZE(RenderCanvas::OnSize)
		EVT_PAINT(RenderCanvas::OnPaint)
		wxEND_EVENT_TABLE()
		RenderCanvas::RenderCanvas(wxWindow* parent,
			const wxGLAttributes& canvasAttrs) : wxGLCanvas(parent,
				canvasAttrs)
	{
		//SetMinSize(FromDIP(m_MinSize));
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
		if (!m_ContextRenderer)
			m_ContextRenderer = std::make_unique<TPContextRenderer>(this);
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
		if (m_ContextRenderer)
		{
			m_ContextRenderer->resize(viewPortSize.x,
				viewPortSize.y); 
		}
		deactiveContext(); 
		event.Skip(); 
	}
	void RenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		activeContext(); 
		if (m_ContextRenderer)
		{
			m_ContextRenderer->update(m_dtTime); 
			m_ContextRenderer->render(); 
		}
		swapBuff(); 
		deactiveContext(); 
	}
	void RenderCanvas::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
}