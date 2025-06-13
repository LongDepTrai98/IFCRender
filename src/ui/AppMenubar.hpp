#ifndef _APP_MENUBAR_HPP_
#define _APP_MENUBAR_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class AppMenubar : public wxMenuBar
	{
	public: 
		AppMenubar(wxFrame* main_window);
	private: 
		wxWindow* m_Window{ nullptr }; 
	};
}
#endif // !_APP_MENUBAR_HPP_
