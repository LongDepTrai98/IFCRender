#ifndef _WINDOW_FRAME_HPP_
#define _WINDOW_FRAME_HPP_
#include <wx/wx.h>
#include <wx/aui/aui.h>
namespace viewer
{
	class SceneView; 
	class WindowFrame : public wxFrame
	{
	public: 
		WindowFrame(); 
	private: 
		void initMenuBar();
		void initTreeCtrl(); 
		void initScene(); 
		void OnHello(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
		void OnInternalIdle() override;
	private: 
		void OnSize(wxSizeEvent& event);
		wxDECLARE_EVENT_TABLE();
	private: 
		wxMenuBar* m_MenuBar{ nullptr };
		std::unique_ptr<SceneView> m_Scene{ nullptr }; 
		wxAuiManager m_Mgr;
	};
}
#endif // !_WINDOW_FRAME_HPP_
