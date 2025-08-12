#include <iostream>
#include <GL/glew.h>
#include "BimRenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "view/ViewportGizmo.hpp"
#include "core/IWindowEventHandler.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "core/lock/ContextLock.hpp"
namespace dragon
{
	BimRenderCanvas::BimRenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs) : IGLCanvas(parent,
		canvasAttrs)
	{
		/*INIT UI FOR RENDERER*/
		ctxAttrs.PlatformDefaults()
			.CoreProfile()
			.OGLVersion(3, 3)
			.EndList();
		initGLContext();
		m_ContextLock->lock(); 
		initContextRenderer();
		initMenu();
		bindFunction();
		m_ContextLock->unlock(); 
	}
	BimRenderCanvas::~BimRenderCanvas()
	{
	}
	void BimRenderCanvas::OnCallbackToolbarCommand(ToolBarData& data)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnToolBarClick(data);
	}
	void BimRenderCanvas::OnMenu(wxCommandEvent& event)
	{
		SetCurrent(*m_Context); 
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		MenuData data;
		data.event_id = event.GetId();
		if (event_handler)
			event_handler->OnMenuClick(data);
		wglMakeCurrent(NULL, NULL); 
	}
	void BimRenderCanvas::initContextRenderer()
	{
		if (!m_Renderer)
			m_Renderer = std::make_unique<THREEPPRenderer>(this);
	}
	void BimRenderCanvas::initUI()
	{
		auto button = new wxButton(this, wxID_ANY, "MSAA", wxPoint(10, 10), wxSize(150, 30));
	}
	void BimRenderCanvas::initMenu()
	{
		main_menu.Append((int)ID_EVENT::MAIN_MENU_HIDE, "Hide");
	}
	void BimRenderCanvas::bindFunction()
	{
		/*CANVAS EVENT*/
		Bind(wxEVT_PAINT, &BimRenderCanvas::OnPaint, this);
		Bind(wxEVT_SIZE, &BimRenderCanvas::OnSize, this);
		Bind(wxEVT_MOTION, &BimRenderCanvas::OnMouseMove, this);
		Bind(wxEVT_LEFT_DOWN, &BimRenderCanvas::OnMousePress, this);
		Bind(wxEVT_RIGHT_DOWN, &BimRenderCanvas::OnMousePress, this);
		Bind(wxEVT_LEFT_UP, &BimRenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_RIGHT_UP, &BimRenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_MOUSEWHEEL, &BimRenderCanvas::OnMouseWheel, this);
		Bind(wxEVT_KEY_UP, &BimRenderCanvas::OnKeyUp, this);
		Bind(wxEVT_KEY_DOWN, &BimRenderCanvas::OnKeyDown, this);
		/*MENU*/
		Bind(wxEVT_MENU, &BimRenderCanvas::OnMenu, this, static_cast<int>(ID_EVENT::MAIN_MENU_HIDE));
		/*BUTTON*/
		Bind(wxEVT_BUTTON, &BimRenderCanvas::OnClickEnableMSAA, this);
	}
	void BimRenderCanvas::swapBuff()
	{
		SwapBuffers();
	}
	wxSize BimRenderCanvas::getSize()
	{
		return GetSize() * GetContentScaleFactor();
	}
	IRenderer* BimRenderCanvas::getRenderer()
	{
		return m_Renderer.get();
	}
	void BimRenderCanvas::Invalidate()
	{
		m_bIsDirty = true;
	}
	void BimRenderCanvas::OnSize(wxSizeEvent& event)
	{
		m_ContextLock->lock(); 
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		if (m_Renderer)
		{
			static_cast<THREEPPRenderer*>(m_Renderer.get())->resize(viewPortSize.x,
				viewPortSize.y);
		}
		Invalidate();
		m_ContextLock->unlock(); 
		event.Skip();
	}
	void BimRenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		m_ContextLock->lock(); 
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
		m_ContextLock->unlock(); 
	}
	void BimRenderCanvas::OnMouseMove(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
		{
			event_handler->OnMouseMove(event);
		}
		Invalidate();
		event.Skip();
	}
	void BimRenderCanvas::OnMousePress(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		int buttonFlag = event.GetButton();
		if (wxMOUSE_BTN_RIGHT == buttonFlag)
		{
			if (!m_bIsHoldCtrl)
			{
				event.Skip();
				return;
			}
		}
		if (event_handler)
		{
			event_handler->OnMousePress(event);
			event_handler->isMouseDown = true;
		}
		Invalidate();
		event.Skip();
	}
	void BimRenderCanvas::OnMouseRelease(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		int buttonFlag = event.GetButton();
		if (wxMOUSE_BTN_RIGHT == buttonFlag)
		{
			if (!m_bIsHoldCtrl)
			{
				if (wxMOUSE_BTN_RIGHT == buttonFlag)
				{
					PopupMenu(&main_menu, event.GetPosition());
					event.Skip();
				}
			}
			else
			{
				event.Skip();
			}
			return;
		}
		if (event_handler)
		{
			event_handler->OnMouseRelease(event);
			event_handler->isMouseDown = false;
		}

		Invalidate();
		event.Skip();
	}
	void BimRenderCanvas::OnMouseWheel(wxMouseEvent& event)
	{
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnMouseWheel(event);
		Invalidate();
		event.Skip();
	}
	void BimRenderCanvas::OnClickEnableMSAA(wxCommandEvent& command)
	{
	}
	void BimRenderCanvas::OnKeyDown(wxKeyEvent& event)
	{
		if (event.GetKeyCode() == WXK_CONTROL)
		{
			m_bIsHoldCtrl = true;
		}
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnKeyDown(event);
		event.Skip();
	}
	void BimRenderCanvas::OnKeyUp(wxKeyEvent& event)
	{
		if (event.GetKeyCode() == WXK_CONTROL)
		{
			m_bIsHoldCtrl = false;
		}
		WindowEventHandler* event_handler = static_cast<WindowEventHandler*>(static_cast<THREEPPRenderer*>(m_Renderer.get()));
		if (event_handler)
			event_handler->OnKeyUp(event);
		event.Skip();
	}
	void BimRenderCanvas::OnInternalIdle()
	{
		wxWindow::OnInternalIdle();
		Refresh(false);
	}
	void BimRenderCanvas::enableMultisampling()
	{
		glEnable(GL_MULTISAMPLE);
	}
	void BimRenderCanvas::disableMultisampling()
	{
		glDisable(GL_MULTISAMPLE);
	}
}