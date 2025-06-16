#ifndef _APP_COMMAND_HANDLER_HPP_
#define _APP_COMMAND_HANDLER_HPP_
namespace dragon
{
	class wxWindow; 
	class AppCommandHandler
	{
	public: 
		AppCommandHandler(wxWindow* parent);
		~AppCommandHandler() = default; 
	private: 
		wxWindow* m_ParentWindow{ nullptr }; 
	};
}
#endif // !_APP_COMMAND_HANDLER_HPP_
