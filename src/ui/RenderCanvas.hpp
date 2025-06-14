#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include "wxInclude.hpp"
#include "threepp/canvas/WindowSize.hpp"
#include <memory>
namespace dragon
{
	class IRenderer;
	class ViewPortGizmo;
	class RenderCanvas : public wxGLCanvas
	{
	public:
		RenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~RenderCanvas();
	private:
		void initGLContext();
		void initContextRenderer();
		void initUI();
		void bindFunction();
	private:
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnMouseMove(wxMouseEvent& event);
		void OnMousePress(wxMouseEvent& event);
		void OnMouseRelease(wxMouseEvent& event);
		void OnMouseWheel(wxMouseEvent& event);
		void OnClickEnableMSAA(wxCommandEvent& command);
		void OnInternalIdle() override;
		/*RENDER*/
	private:
		void enableMultisampling();
		void disableMultisampling();
	public:
		void activeContext();
		void deactiveContext();
		void swapBuff();
		wxSize getSize();
	private:
		//main context
		std::unique_ptr<wxGLContext> m_Context{ nullptr };
		/* MAIN SCENE*/
		std::unique_ptr<IRenderer> m_Renderer{ nullptr };
		/*GIZMO SCENE*/
		std::unique_ptr<ViewPortGizmo> m_ViewPortGizmo{ nullptr };
		//min size
		wxSize m_MinSize{ 640, 480 };
		float  m_dtTime{ 0.0 };
	};
}
#endif // !_SCENE_VIEW_HPP_