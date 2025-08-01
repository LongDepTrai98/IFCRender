#ifndef _APP_TOOL_BAR_HPP_
#define _APP_TOOL_BAR_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class AppToolBar
	{
	public:
		AppToolBar(wxAuiToolBar* toolbar);
		AppToolBar(wxWindow* main);
	public: 
		wxAuiToolBar* m_ToolBar{ nullptr };
	private:
		void AddTool();
	private:
		wxBitmapButton* m_bpOpenFileButton{ nullptr };
	};
}
#endif // !_APP_TOOL_BAR_HPP_