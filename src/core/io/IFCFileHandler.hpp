#ifndef _IFC_FILE_HANDLER_HPP_
#define _IFC_FILE_HANDLER_HPP_
#include "core/io/IFileHandler.hpp"
class GeometryConverter;
class GeometrySettings; 
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
		std::shared_ptr<GeometryConverter>& getGeometryConverter(); 
	private:
		/*PRIVATE PARAM*/
		std::shared_ptr<GeometryConverter> m_GeometryConverter{ nullptr };
		std::shared_ptr<GeometrySettings> m_GeometrySettings{ nullptr }; 
		double m_Eps{ 1.5e-9 };
	};
}
#endif // !_IFC_FILE_HANDLER_HPP_