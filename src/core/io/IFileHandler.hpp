#ifndef _FILE_HANDLER_HPP_
#define _FILE_HANDLER_HPP_
#include <filesystem>
#include "wxInclude.hpp"
namespace dragon
{
	class FileHandler
	{
	public:
		virtual ~FileHandler() = default;
	public: 
		/*SET WINDOW TO GET CURRENT VIEWPORT*/
		void setViewport(wxWindow* m_Window); 
		virtual void open(const std::filesystem::path& file_path) = 0;
	protected: 
		wxWindow* m_Window{ nullptr }; 
	};
}
#endif // !_FILE_HANDLER_HPP_