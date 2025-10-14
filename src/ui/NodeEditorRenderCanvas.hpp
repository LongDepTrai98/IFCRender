#ifndef _NODE_EDITOR_RENDER_CANVAS_HPP_
#define _NODE_EDITOR_RENDER_CANVAS_HPP_
#include "ui/IGLCanvas.hpp"
#include "input/input.hpp"
#include <memory>
#include <functional>
//#include "graph/WindowData.hpp"
//#include "graph/Editor.hpp"
namespace dragon
{
	class IRenderer;
	class ViewPortGizmo;
	class ToolBarData;
	class ContextLock;
	class NodeEditorRenderCanvas : public IGLCanvas
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
	};
}
#endif // !_NODE_EDITOR_RENDER_CANVAS_HPP_
