#ifndef _WINDOW_FRAME_HPP_
#define _WINDOW_FRAME_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class RenderCanvas; 
	class WindowFrame : public wxFrame
	{
	public: 
		WindowFrame(); 
	private: 
		void initUIManager(); 
		void initMenuBar();
		void initTreeCtrl(); 
		void initScene(); 
		void OnHello(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
	private: 
		void OnSize(wxSizeEvent& event);
		wxDECLARE_EVENT_TABLE();
	private: 
		wxMenuBar* m_MenuBar{ nullptr };
		std::unique_ptr<RenderCanvas> m_RenderCanvas{ nullptr }; 
		std::unique_ptr<wxAuiManager> m_UIManager{ nullptr };
	};
}
#endif // !_WINDOW_FRAME_HPP_
