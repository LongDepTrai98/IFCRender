#ifndef _IFC_FILE_HANDLER_HPP_
#define _IFC_FILE_HANDLER_HPP_
#include "core/io/IFileHandler.hpp"
namespace threepp
{
	class Scene; 
	class Group; 
}
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
		void addLight(threepp::Scene& scene, std::shared_ptr<threepp::Group>& container);
		void createPlane(threepp::Scene& scene);
	private:
		/*PRIVATE PARAM*/
		std::shared_ptr<GeometryConverter> m_GeometryConverter{ nullptr };
		std::shared_ptr<GeometrySettings> m_GeometrySettings{ nullptr }; 
		double m_Eps{ 1.5e-9 };
		bool m_bIsCreateInstance{ true }; 
	};
}
#endif // !_IFC_FILE_HANDLER_HPP_