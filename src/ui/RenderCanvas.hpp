#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include <memory>
#include "wxInclude.hpp"
#include "threepp/canvas/WindowSize.hpp"
namespace dragon
{
	class IRenderer;
	class RenderCanvas : public wxGLCanvas
	{
	public: 
		RenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~RenderCanvas();
	private: 
		void initGLContext(); 
		void initContextRenderer(); 
		void bindFunction(); 
	private: 
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event); 
		void OnMouseMove(wxMouseEvent& event);
		void OnMousePress(wxMouseEvent& event);
		void OnMouseRelease(wxMouseEvent& event);
		void OnMouseWheel(wxMouseEvent& event);
		void OnInternalIdle() override;
	public: 
		void activeContext();
		void deactiveContext();
		void swapBuff();
		wxSize getSize();
	private:
		//main context 
		std::unique_ptr<wxGLContext> m_Context{nullptr};
		//main render context 
		std::unique_ptr<IRenderer> m_Renderer{ nullptr };
		//min size 
		wxSize m_MinSize{ 640, 480 };
		float  m_dtTime{ 0.0 }; 
	};
}
#endif // !_SCENE_VIEW_HPP_
