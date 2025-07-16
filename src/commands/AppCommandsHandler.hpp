#ifndef _APP_COMMAND_HANDLER_HPP_
#define _APP_COMMAND_HANDLER_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class AppCommandHandler
	{
	public:
		AppCommandHandler(wxWindow* parent);
		~AppCommandHandler() = default;
	public:
		void OnOpenFile(wxCommandEvent& event);
	private:
		wxWindow* m_ParentWindow{ nullptr };
	};
}
#endif // !_APP_COMMAND_HANDLER_HPP_