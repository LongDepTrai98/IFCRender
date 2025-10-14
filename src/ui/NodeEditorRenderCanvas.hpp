#ifndef _NODE_EDITOR_RENDER_CANVAS_HPP_
#define _NODE_EDITOR_RENDER_CANVAS_HPP_
#include "ui/IGLCanvas.hpp"
#include "input/input.hpp"
#include "graph/platform.h"
#include <memory>
#include <functional>
//#include "graph/WindowData.hpp"
//#include "graph/Editor.hpp"
class NodeApplication;
namespace dragon
{
	class IRenderer;
	class ViewPortGizmo;
	class ToolBarData;
	class ContextLock;
	class NodeEditorRenderCanvas : public IGLCanvas, 
		public Platform
	{
	public: 
		NodeEditorRenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~NodeEditorRenderCanvas() override; 
	public: 
		void OnCallbackToolbarCommand(ToolBarData& data) override;
		void OnInternalIdle() override; 
		wxSize getSize() override;
		void Invalidate() override;
		virtual WXLRESULT MSWWindowProc(
			WXUINT msg,
			WXWPARAM wParam,
			WXLPARAM lParam
		) override; 
	public: 
		void bindFunction(); 
	public: 
		///override 
		/// 
		bool ApplicationStart(int argc, char** argv) override;
		void ApplicationStop() override;
		bool OpenMainWindow(const char* title, int width, int height) override;
		bool CloseMainWindow() override;
		void* GetMainWindowHandle() const override;
		void SetMainWindowTitle(const char* title) override;
		void ShowMainWindow() override;
		bool ProcessMainWindowEvents() override;
		bool IsMainWindowVisible() const override;
		void SetRenderer(AppRenderer* renderer) override;
		void NewFrame() override;
		void FinishFrame() override;
		void Quit() override;
	/// <summary>
	/// Event 
	/// </summary>
	private: 
		//graph::OGL_Window_Data data; 
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnMouseMove(wxMouseEvent& event);
		void OnMousePress(wxMouseEvent& event);
		void OnMouseRelease(wxMouseEvent& event);
		void OnMouseWheel(wxMouseEvent& event);
		void OnKeyDown(wxKeyEvent& command);
		void OnKeyUp(wxKeyEvent& command);
		void OnIdle(wxIdleEvent& event); 
	private: 
		std::unique_ptr<NodeApplication> application{ nullptr };
		HWND m_MainWindowHandle; 
		AppRenderer* m_Renderer{ nullptr }; 
	};
}
#endif // !_NODE_EDITOR_RENDER_CANVAS_HPP_
