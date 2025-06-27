#ifndef _WEB_IFC_CONVERTER_HPP_
#define _WEB_IFC_CONVERTER_HPP_
#include <filesystem>
#include <memory>
//#include "web-ifc/parsing/IfcLoader.h"
//#include "web-ifc/geometry/IfcGeometryProcessor.h"

namespace threepp
{
	class Group; 
	class BufferGeometry;
	class Material;
	class Mesh; 
}

namespace webifc::parsing {
	class IfcLoader; 
}
namespace webifc::geometry{
	class IfcGeometryProcessor; 
}

namespace dragon
{
	class WebIFCConverter
	{
	public: 
		std::shared_ptr<threepp::Group> convert(const std::filesystem::path& path);
	private: 
		void loadAllMesh(webifc::parsing::IfcLoader& loader,
			webifc::geometry::IfcGeometryProcessor& geometryLoader,
			std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries,
			std::vector<std::shared_ptr<threepp::Material>>& materials);
		std::shared_ptr<threepp::Mesh> Mergeo(const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries, const std::vector<std::shared_ptr<threepp::Material>>& materials); 
	};
}
#endif // !_WEB_IFC_CONVERTER_HPP_
