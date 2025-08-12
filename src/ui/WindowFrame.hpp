#ifndef _WINDOW_FRAME_HPP_
#define _WINDOW_FRAME_HPP_
#include "wxInclude.hpp"

class customDockArt : public wxAuiDefaultDockArt
{
public: 
	void setStyle()
	{
		m_gradientType = wxAUI_GRADIENT_NONE;
	}
};

namespace dragon
{
	class AppMenubar;
	class BimRenderCanvas;
	class MapRenderCanvas; 
	class AppCommandHandler;
	class ElementTreeCtrl;
	class AppToolBar;
	class ToolBarData;
	class WindowFrame : public wxFrame
	{
	public:
		WindowFrame();
	public:
		BimRenderCanvas* getBimRenderCanvas();
		ElementTreeCtrl* getElementTreeCtrl();
		void OnCallbackToolbarCommand(ToolBarData& data);
		void OnResize(wxSizeEvent& event);
		wxAuiToolBar* getCustomToolBar(); 
	private:
		void initUI();
		void initUIManager();
		void initMenuBar();
		void initTreeCtrl();
		void initAppToolBar();
		void initBimRenderCanvas();
		void initMapRenderCanvas(); 
		void initCommand();
	private:
		AppMenubar* m_AppMenuBar{ nullptr };
		ElementTreeCtrl* m_ElementTreeCtrl{ nullptr };
		std::unique_ptr<AppToolBar> m_AppToolBar{ nullptr };
		std::unique_ptr<BimRenderCanvas> m_BimRenderCanvas{ nullptr };
		std::unique_ptr<MapRenderCanvas> m_MapRenderCanvas{ nullptr }; 
		std::unique_ptr<wxAuiManager> m_UIManager{ nullptr };
		std::unique_ptr<AppCommandHandler> m_CommandHandler{ nullptr };
		bool m_bIsMaximize{ true };
	};
}
#endif // !_WINDOW_FRAME_HPP_