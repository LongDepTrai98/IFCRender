#ifndef _SCENE_VIEW_HPP_
#define _SCENE_VIEW_HPP_
#include <GL/glew.h> 
#include <wx/glcanvas.h>
#include <memory>
namespace viewer
{
	class SceneView : public wxGLCanvas
	{
	public: 
		SceneView(wxWindow* parent, const wxGLAttributes& canvasAttrs);
		~SceneView(); 
	private: 
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event); 
		wxDECLARE_EVENT_TABLE();
	private:
		//main context 
		std::unique_ptr<wxGLContext> m_Context{nullptr};
		//min size 
		wxSize m_MinSize{ 300, 300 };
	};
}
#endif // !_SCENE_VIEW_HPP_
