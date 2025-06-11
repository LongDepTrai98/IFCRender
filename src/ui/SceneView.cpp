#include "SceneView.hpp"
#include "threepp/threepp.hpp"
#include <iostream>
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
		wxGLContextAttrs ctxAttrs; 
		ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
		//create context 
		if (!m_Context)
			m_Context = std::make_unique<wxGLContext>(this, nullptr, &ctxAttrs);
		if (!m_Context->IsOK())
		{
			throw std::exception("Can't create context renderer"); 
		}
		SetMinSize(FromDIP(m_MinSize));
	}
	SceneView::~SceneView() = default; 
	void SceneView::OnSize(wxSizeEvent& event)
	{
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		if (m_Context)
		{
			glViewport(0, 0, viewPortSize.x, viewPortSize.y);
		}
		event.Skip(); 
	}
	void SceneView::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		SetCurrent(*m_Context);
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		SwapBuffers(); 
	}
	void SceneView::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
}