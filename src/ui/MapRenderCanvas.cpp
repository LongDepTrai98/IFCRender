#include "MapRenderCanvas.hpp"

namespace dragon
{
	MapRenderCanvas::MapRenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs) : 
		wxGLCanvas(parent,
			canvasAttrs)
	{
		initGLContext(); 
		SetCurrent(*m_Context); 
		initContextMap(); 
		bindFunction(); 
	}
	MapRenderCanvas::~MapRenderCanvas()
	{
	}
	void MapRenderCanvas::initGLContext()
	{
		wxGLContextAttrs ctxAttrs;
		ctxAttrs.PlatformDefaults()
			.CoreProfile()
			.OGLVersion(3, 3)
			.EndList();
		if (!m_Context)
			m_Context = std::make_unique<wxGLContext>(this, nullptr, &ctxAttrs);
		if (!m_Context->IsOK())
		{
			throw std::exception("Can't create context renderer");
		}
	}
	void MapRenderCanvas::initContextMap()
	{
	}
	void MapRenderCanvas::bindFunction()
	{
		Bind(wxEVT_PAINT, &MapRenderCanvas::OnPaint, this);
		Bind(wxEVT_SIZE, &MapRenderCanvas::OnSize, this);
	}
	void MapRenderCanvas::OnSize(wxSizeEvent& event)
	{
		SetCurrent(*m_Context);
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		glViewport(0.0, 0.0, viewPortSize.GetWidth(), viewPortSize.GetHeight()); 
		wglMakeCurrent(NULL, NULL);
		event.Skip();
	}
	void MapRenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		SetCurrent(*m_Context);
		glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		SwapBuffers();
		wglMakeCurrent(NULL, NULL); 
	}
	void MapRenderCanvas::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
	void MapRenderCanvas::activeContext()
	{
		SetCurrent(*m_Context); 
	}
}