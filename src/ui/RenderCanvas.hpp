#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include <memory>
#include "wxInclude.hpp"
#include "threepp/canvas/WindowSize.hpp"
namespace dragon
{
	class IContextRenderer; 
	class RenderCanvas : public wxGLCanvas
	{
	public: 
		RenderCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~RenderCanvas();
	private: 
		void initGLContext(); 
		void initContextRenderer(); 
	private: 
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event); 
		void OnInternalIdle() override;
		wxDECLARE_EVENT_TABLE();
	public: 
		void activeContext();
		void deactiveContext();
		void swapBuff();
		wxSize getSize();
	private:
		//main context 
		std::unique_ptr<wxGLContext> m_Context{nullptr};
		//main render context 
		std::unique_ptr<IContextRenderer> m_ContextRenderer;
		//min size 
		wxSize m_MinSize{ 640, 480 };
		float  m_dtTime{ 0.0 }; 
	};
}
#endif // !_SCENE_VIEW_HPP_
