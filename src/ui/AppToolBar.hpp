#ifndef _APP_TOOL_BAR_HPP_
#define _APP_TOOL_BAR_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class AppToolBar
	{
	public:
		AppToolBar(wxToolBar* toolbar);
	private:
		wxBitmapButton* m_bpOpenFileButton{ nullptr };
		wxToolBar* m_ToolBar{ nullptr };
	};
}
#endif // !_APP_TOOL_BAR_HPP_