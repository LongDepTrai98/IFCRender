#ifndef _IFC_FILE_HANDLER_HPP_
#define _IFC_FILE_HANDLER_HPP_
#include "core/io/IFileHandler.hpp"
namespace dragon
{
	class IFCFileHandler : public FileHandler
	{
	public:
		IFCFileHandler();
		~IFCFileHandler() override;
	public:
		/*OVERRIDE METHOD*/
		void open(const std::filesystem::path& file_path) override;
	private:
		/*PRIVATE PARAM*/
	};
}
#endif // !_IFC_FILE_HANDLER_HPP_