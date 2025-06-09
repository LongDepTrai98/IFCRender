#include "SceneView.hpp"
#include <iostream>
#include <wx/dcclient.h>


namespace viewer
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
			m_Context = new wxGLContext(this,
				nullptr,
				&ctxAttrs);
		if (!m_Context->IsOK())
		{
			throw std::exception("Can't create context renderer"); 
		}
		SetMinSize(FromDIP(m_MinSize));
		m_Hdc = this->m_hDC; 
		m_Hglrc = m_Context->GetGLRC(); 
	}
	SceneView::~SceneView()
	{
		delete m_Context; 
		m_Context = nullptr; 
	}
	void SceneView::OnSize(wxSizeEvent& event)
	{
		if (m_Context)
		{
			glViewport(0, 
				0, 
				event.GetSize().x,
				event.GetSize().y);
		}
		event.Skip(); 
	}
	void SceneView::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		wglMakeCurrent(dc.GetHDC(), m_Hglrc);
		/*glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		SwapBuffers();*/
		//::SwapBuffers(m_Hdc); 
		event.Skip(); 
	}
}