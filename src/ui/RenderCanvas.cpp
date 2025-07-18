#include <iostream>
#include <GL/glew.h>
#include "RenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "view/ViewportGizmo.hpp"
#include "core/IWindowEventHandler.hpp"
#include "input/input.hpp"
namespace dragon
{
	RenderCanvas::RenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs) : wxGLCanvas(parent,
		canvasAttrs)
	{
		/*INIT UI FOR RENDERER*/
		//initUI();
		initGLContext();
		initContextRenderer();
		/*BIND FUNCTION*/
		bindFunction();
	}
	RenderCanvas::~RenderCanvas()
	{
	}
	void RenderCanvas::OnCallbackToolbarCommand(ToolBarData& data)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnToolBarClick(data); 
	}
	void RenderCanvas::initGLContext()
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
	void RenderCanvas::initContextRenderer()
	{
		if (!m_Renderer)
			m_Renderer = std::make_unique<THREEPPRenderer>(this);
	}
	void RenderCanvas::initUI()
	{
		auto button = new wxButton(this, wxID_ANY, "MSAA", wxPoint(10, 10), wxSize(150, 30));
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
		Bind(wxEVT_KEY_UP, &RenderCanvas::OnKeyUp, this);
		Bind(wxEVT_KEY_DOWN, &RenderCanvas::OnKeyDown, this);
		/*BUTTON*/
		Bind(wxEVT_BUTTON, &RenderCanvas::OnClickEnableMSAA, this);
	}
	void RenderCanvas::activeContext()
	{
		SetCurrent(*m_Context);
	}
	void RenderCanvas::deactiveContext()
	{
		wglMakeCurrent(nullptr, nullptr);
	}
	void RenderCanvas::swapBuff()
	{
		SwapBuffers();
	}
	wxSize RenderCanvas::getSize()
	{
		return GetSize() * GetContentScaleFactor();
	}
	IRenderer* RenderCanvas::getRenderer()
	{
		return m_Renderer.get();
	}
	void RenderCanvas::Invalidate()
	{
		m_bIsDirty = true;
	}
	void RenderCanvas::OnSize(wxSizeEvent& event)
	{
		activeContext();
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		if (m_Renderer)
		{
			static_cast<THREEPPRenderer*>(m_Renderer.get())->resize(viewPortSize.x,
				viewPortSize.y);
		}
		deactiveContext();
		Invalidate(); 
		event.Skip();
	}
	void RenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		activeContext();
		if (m_bIsDirty)
		{
			enableMultisampling();
			if (m_Renderer)
			{
				m_Renderer->update(m_dtTime);
				static_cast<THREEPPRenderer*>(m_Renderer.get())->render();
			}
			swapBuff();
			disableMultisampling();
			m_bIsDirty = false; 
		}
		deactiveContext();
	}
	void RenderCanvas::OnMouseMove(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
		{
			event_handler->OnMouseMove(event);
			if (event_handler->isMouseDown)
				Invalidate(); 
		}
		event.Skip();
	}
	void RenderCanvas::OnMousePress(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
		{
			event_handler->OnMousePress(event);
			event_handler->isMouseDown = true; 
		}
		Invalidate(); 
		event.Skip();
	}
	void RenderCanvas::OnMouseRelease(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
		{
			event_handler->OnMouseRelease(event);
			event_handler->isMouseDown = false;
		}
		Invalidate();
		event.Skip();
	}
	void RenderCanvas::OnMouseWheel(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnMouseWheel(event);
		Invalidate();
		event.Skip();
	}
	void RenderCanvas::OnClickEnableMSAA(wxCommandEvent& command)
	{

	}
	void RenderCanvas::OnKeyDown(wxKeyEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnKeyDown(event);
		event.Skip();
	}
	void RenderCanvas::OnKeyUp(wxKeyEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnKeyUp(event);
		event.Skip(); 
	}
	void RenderCanvas::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
	void RenderCanvas::enableMultisampling()
	{
		glEnable(GL_MULTISAMPLE);
	}
	void RenderCanvas::disableMultisampling()
	{
		glDisable(GL_MULTISAMPLE);
	}
}