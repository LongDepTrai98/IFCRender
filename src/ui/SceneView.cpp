#include "SceneView.hpp"
#include <iostream>
#include "core/context/TPContextRenderer.hpp"
namespace dragon
{
	wxBEGIN_EVENT_TABLE(SceneView, wxGLCanvas)
		EVT_SIZE(SceneView::OnSize)
		EVT_PAINT(SceneView::OnPaint)
		wxEND_EVENT_TABLE()
		SceneView::SceneView(wxWindow* parent,
			const wxGLAttributes& canvasAttrs) : wxGLCanvas(parent,
				canvasAttrs)
	{
		initGLContext(); 
		initContextRenderer(); 
		SetMinSize(FromDIP(m_MinSize));
	}
	SceneView::~SceneView()
	{
	}
	void SceneView::initGLContext()
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
	void SceneView::initContextRenderer()
	{
		if (!m_ContextRenderer)
			m_ContextRenderer = std::make_unique<TPContextRenderer>(this);
	}
	void SceneView::activeContext()
	{
		SetCurrent(*m_Context);
	}
	void SceneView::deactiveContext()
	{
		wglMakeCurrent(NULL, NULL);
	}
	void SceneView::swapBuff()
	{
		SwapBuffers(); 
	}
	wxSize SceneView::getSize()
	{
		return GetSize() * GetContentScaleFactor();
	}
	void SceneView::OnSize(wxSizeEvent& event)
	{
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		if (m_Context)
		{
			glViewport(0, 
				0,
				viewPortSize.x, 
				viewPortSize.y);
		}
		if (m_ContextRenderer)
		{
			m_ContextRenderer->resize(viewPortSize.x,
				viewPortSize.y); 
		}
		event.Skip(); 
	}
	void SceneView::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		if (m_ContextRenderer)
		{
			m_ContextRenderer->update(m_dtTime); 
			m_ContextRenderer->render(); 
		}
	}
	void SceneView::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
}