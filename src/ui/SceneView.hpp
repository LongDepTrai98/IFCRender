#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include <memory>
#include "wxInclude.hpp"
namespace dragon
{
	class IContextRenderer; 
	class SceneView : public wxGLCanvas
	{
	public: 
		SceneView(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~SceneView();
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
	private:
		//main context 
		std::unique_ptr<wxGLContext> m_Context{nullptr};
		//main render context 
		std::unique_ptr<IContextRenderer> m_ContextRenderer;
		//min size 
		wxSize m_MinSize{ 300, 300 };
		float  m_dtTime{ 0.0 }; 
	};
}
#endif // !_SCENE_VIEW_HPP_
