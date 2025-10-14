#include "NodeEditorRenderCanvas.hpp"
#include "core/IWindowEventHandler.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "core/lock/ContextLock.hpp"
#include "graph/NodeApp.hpp"
#include "graph/imgui_impl_win32.h"
#include "graph/AppRenderer.hpp"
//#include <imgui.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
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
		//create imgui context 
		if (!application)
		{
			application = std::make_unique<NodeApplication>(this,
				m_Name.c_str()); 
		}
		wxSize windowSize = this->GetSize();
		application->Create(windowSize.GetWidth(), windowSize.GetHeight()); 
		OpenMainWindow(m_Name.c_str(), windowSize.GetWidth(), windowSize.GetHeight());
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
		if (application)
		{
			if (ImGui_ImplWin32_WndProcHandler((HWND)this->GetHWND(), (UINT)msg, (WPARAM)wParam, (LPARAM)lParam))
				return 1;
		}
		return wxGLCanvas::MSWWindowProc(msg, wParam, lParam);
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
		Bind(wxEVT_IDLE, &NodeEditorRenderCanvas::OnIdle, this);
	}
	bool NodeEditorRenderCanvas::ApplicationStart(int argc, char** argv)
	{
		return false;
	}
	void NodeEditorRenderCanvas::ApplicationStop()
	{
	}
	bool NodeEditorRenderCanvas::OpenMainWindow(const char* title, int width, int height)
	{
		m_MainWindowHandle = (HWND)this->GetHWND(); 
		if (!m_MainWindowHandle)
			return false; 
		if (!ImGui_ImplWin32_Init(m_MainWindowHandle))
		{
			DestroyWindow(m_MainWindowHandle);
			m_MainWindowHandle = nullptr;
			return false;
		}
		//SetDpiScale(ImGui_ImplWin32_GetDpiScaleForHwnd(m_MainWindowHandle));
		return true;
	}
	bool NodeEditorRenderCanvas::CloseMainWindow()
	{
		return false;
	}
	void* NodeEditorRenderCanvas::GetMainWindowHandle() const
	{
		return nullptr;
	}
	void NodeEditorRenderCanvas::SetMainWindowTitle(const char* title)
	{
	}
	void NodeEditorRenderCanvas::ShowMainWindow()
	{
	}
	bool NodeEditorRenderCanvas::ProcessMainWindowEvents()
	{
		return false;
	}
	bool NodeEditorRenderCanvas::IsMainWindowVisible() const
	{
		return false;
	}
	void NodeEditorRenderCanvas::SetRenderer(AppRenderer* renderer)
	{
		m_Renderer = renderer; 
	}
	void NodeEditorRenderCanvas::NewFrame()
	{
		ImGui_ImplWin32_NewFrame();
	}
	void NodeEditorRenderCanvas::FinishFrame()
	{
		if (m_Renderer)
			m_Renderer->Present();
	}
	void NodeEditorRenderCanvas::Quit()
	{
	}
	void NodeEditorRenderCanvas::OnFrame(const float& time)
	{

	}
	void NodeEditorRenderCanvas::OnSize(wxSizeEvent& event)
	{
		if (!application) return; 
		m_ContextLock->lock(); 
		auto viewPortSize = event.GetSize() * GetContentScaleFactor();
		application->getRenderer()->Resize(viewPortSize.x,viewPortSize.y); 
		m_ContextLock->unlock(); 
		event.Skip();
	}
	void NodeEditorRenderCanvas::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		m_ContextLock->lock(); 
		if (application)
			application->Frame(); 
		SwapBuffers();
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
	void NodeEditorRenderCanvas::OnIdle(wxIdleEvent& event)
	{
		Refresh(false);
		event.RequestMore();
	}
}