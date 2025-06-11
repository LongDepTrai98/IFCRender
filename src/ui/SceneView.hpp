#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include <memory>
#include "wxInclude.hpp"
#include "context/ContextRenderer.hpp"
namespace dragon
{
	class SceneView : public wxGLCanvas
	{
	public: 
		SceneView(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~SceneView(); 
	private: 
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event); 
		void OnInternalIdle() override;
		wxDECLARE_EVENT_TABLE();
	private:
		//main context 
		std::unique_ptr<wxGLContext> m_Context{nullptr};
		//main render context 
		std::unique_ptr<ContextRenderer> m_ContextRenderer{ nullptr }; 
		//min size 
		wxSize m_MinSize{ 300, 300 };
	};
}
#endif // !_SCENE_VIEW_HPP_
