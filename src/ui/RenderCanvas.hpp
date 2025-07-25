#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include "wxInclude.hpp"
#include "threepp/canvas/WindowSize.hpp"
#include <memory>
namespace dragon
{
	class IRenderer;
	class ViewPortGizmo;
	class ToolBarData;
	class RenderCanvas : public wxGLCanvas
	{
	public:
		RenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~RenderCanvas();
	public:
		void OnCallbackToolbarCommand(ToolBarData& data);
		void OnMenu(wxCommandEvent& event);
	private:
		void initGLContext();
		void initContextRenderer();
		void initUI();
		void initMenu();
		void bindFunction();
	private:
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnMouseMove(wxMouseEvent& event);
		void OnMousePress(wxMouseEvent& event);
		void OnMouseRelease(wxMouseEvent& event);
		void OnMouseWheel(wxMouseEvent& event);
		void OnClickEnableMSAA(wxCommandEvent& command);
		void OnKeyDown(wxKeyEvent& command);
		void OnKeyUp(wxKeyEvent& command);
		void OnInternalIdle() override;
		/*RENDER*/
	private:
		void enableMultisampling();
		void disableMultisampling();
	public:
		//void activeContext();
		//void deactiveContext();
		void swapBuff();
		wxSize getSize();
		IRenderer* getRenderer();
		void Invalidate();
	private:
		//main context
		std::unique_ptr<wxGLContext> m_Context{ nullptr };
		/* MAIN SCENE*/
		std::unique_ptr<IRenderer> m_Renderer{ nullptr };
		//min size
		wxSize m_MinSize{ 640, 480 };
		float  m_dtTime{ 0.0 };
		wxGLAttributes m_DispAttrs{};
		bool m_bIsDirty{ true };
		bool m_bIsHoldCtrl{ false };
		wxMenu main_menu{};
	};
}
#endif // !_SCENE_VIEW_HPP_