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
		wxGLContext* m_Context{nullptr};
		//min size 
		wxSize m_MinSize{ 640, 480 };
		HGLRC m_Hglrc; 
		HDC m_Hdc; 
	};
}
#endif // !_SCENE_VIEW_HPP_
