#ifndef _WINDOW_FRAME_HPP_
#define _WINDOW_FRAME_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class AppMenubar;
	class RenderCanvas;
	class AppCommandHandler;
	class ElementTreeCtrl; 
	class WindowFrame : public wxFrame
	{
	public:
		WindowFrame();
	public: 
		RenderCanvas* getRenderCanvas(); 
		ElementTreeCtrl* getElementTreeCtrl(); 
	private:
		void initUI(); 
		void initUIManager();
		void initMenuBar();
		void initTreeCtrl();
		void initScene();
		void initCommand(); 
		void OnHello(wxCommandEvent& event);
		void OnExit(wxCommandEvent& event);
		void OnAbout(wxCommandEvent& event);
	private:
		void OnSize(wxSizeEvent& event);
		wxDECLARE_EVENT_TABLE();
	private:
		AppMenubar* m_AppMenuBar{ nullptr };
		ElementTreeCtrl* m_ElementTreeCtrl{ nullptr }; 
		std::unique_ptr<RenderCanvas> m_RenderCanvas{ nullptr };
		std::unique_ptr<wxAuiManager> m_UIManager{ nullptr };
		std::unique_ptr<AppCommandHandler> m_CommandHandler{ nullptr };
		bool m_bIsMaximize{ true }; 
	};
}
#endif // !_WINDOW_FRAME_HPP_