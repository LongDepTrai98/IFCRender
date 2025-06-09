#ifndef _WINDOW_FRAME_HPP_
#define _WINDOW_FRAME_HPP_
#include <wx/wx.h>
#include <wx/aui/aui.h>
namespace viewer
{
	class WindowFrame : public wxFrame
	{
	public: 
		WindowFrame(); 
	private: 
		void initMenuBar(); 
		void OnHello(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
	private: 
		wxMenuBar* m_MenuBar{ nullptr };
		wxAuiManager m_mgr;
	};
}
#endif // !_WINDOW_FRAME_HPP_
