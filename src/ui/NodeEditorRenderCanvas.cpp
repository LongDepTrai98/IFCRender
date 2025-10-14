#include "NodeEditorRenderCanvas.hpp"
#include "core/IWindowEventHandler.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "core/lock/ContextLock.hpp"
namespace dragon
{
	NodeEditorRenderCanvas::NodeEditorRenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs) : IGLCanvas(parent,
		canvasAttrs)
	{
		m_Name = "node-canvas";
		ctxAttrs.PlatformDefaults()
			.CoreProfile()
			.OGLVersion(3, 3)
			.EndList();
		initGLContext();
		bindFunction(); 
		m_ContextLock->lock();
		m_ContextLock->unlock(); 
	}
	NodeEditorRenderCanvas::~NodeEditorRenderCanvas()
	{
	}
	void NodeEditorRenderCanvas::OnCallbackToolbarCommand(ToolBarData& data)
	{
	}
	void NodeEditorRenderCanvas::OnInternalIdle()
	{
	}
	wxSize NodeEditorRenderCanvas::getSize()
	{
		return GetSize() * GetContentScaleFactor();
	}
	void NodeEditorRenderCanvas::Invalidate()
	{
	}
	WXLRESULT NodeEditorRenderCanvas::MSWWindowProc(WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		return WXLRESULT();
	}
	void NodeEditorRenderCanvas::bindFunction()
	{
		Bind(wxEVT_PAINT, &NodeEditorRenderCanvas::OnPaint, this);
		Bind(wxEVT_SIZE, &NodeEditorRenderCanvas::OnSize, this);
		Bind(wxEVT_MOTION, &NodeEditorRenderCanvas::OnMouseMove, this);
		Bind(wxEVT_LEFT_DOWN, &NodeEditorRenderCanvas::OnMousePress, this);
		Bind(wxEVT_RIGHT_DOWN, &NodeEditorRenderCanvas::OnMousePress, this);
		Bind(wxEVT_LEFT_UP, &NodeEditorRenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_RIGHT_UP, &NodeEditorRenderCanvas::OnMouseRelease, this);
		Bind(wxEVT_MOUSEWHEEL, &NodeEditorRenderCanvas::OnMouseWheel, this);
		Bind(wxEVT_KEY_UP, &NodeEditorRenderCanvas::OnKeyUp, this);
		Bind(wxEVT_KEY_DOWN, &NodeEditorRenderCanvas::OnKeyDown, this);
	}
	void NodeEditorRenderCanvas::OnSize(wxSizeEvent& event)
	{
		event.Skip();
	}
	void NodeEditorRenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		m_ContextLock->lock(); 
		m_ContextLock->unlock(); 
		event.Skip();
	}
	void NodeEditorRenderCanvas::OnMouseMove(wxMouseEvent& event)
	{
		event.Skip();
	}
	void NodeEditorRenderCanvas::OnMousePress(wxMouseEvent& event)
	{
		event.Skip();
	}
	void NodeEditorRenderCanvas::OnMouseRelease(wxMouseEvent& event)
	{
		event.Skip();
	}
	void NodeEditorRenderCanvas::OnMouseWheel(wxMouseEvent& event)
	{
		event.Skip();
	}
	void NodeEditorRenderCanvas::OnKeyDown(wxKeyEvent& command)
	{

	}
	void NodeEditorRenderCanvas::OnKeyUp(wxKeyEvent& command)
	{
	}
}