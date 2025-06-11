#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include <memory>
#include "wxInclude.hpp"
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
		//min size 
		wxSize m_MinSize{ 300, 300 };
	};
}
#endif // !_SCENE_VIEW_HPP_
