#ifndef _WEB_IFC_CONVERTER_HPP_
#define _WEB_IFC_CONVERTER_HPP_
#include <filesystem>
#include <memory>
#include <functional>
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
namespace webifc::geometry {
	class IfcGeometryProcessor;
	class IfcFlatMesh;
	class IfcPlacedGeometry;
	class IfcGeometry;
}
namespace webifc::manager {
	class LoaderSettings;
	class ModelManager;
}

namespace dragon
{
	class WebIFCConverter
	{
	public:
		WebIFCConverter();
		~WebIFCConverter();
	public:
		std::shared_ptr<threepp::Group> convert(const std::filesystem::path& path);
	private:
		void loadAllMesh(webifc::parsing::IfcLoader& loader,
			webifc::geometry::IfcGeometryProcessor& geometryLoader,
			std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries,
			std::vector<std::shared_ptr<threepp::Material>>& materials);
		std::shared_ptr<threepp::Mesh> Mergeo(const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries, const std::vector<std::shared_ptr<threepp::Material>>& materials);
		/*PORT FROM IFC API*/
		void parseIfcFile(const std::string& buffer);
		void loadAllGeometry(const uint32_t& modelID);
		void streamAllMeshes(const uint32_t& modelID);
		void streamAllMeshesWithTypes(const uint32_t& modelID, const std::vector<uint32_t>& types);
		void streamMeshes(const uint32_t& modelId, const std::vector<uint32_t>& expressIds);
		void streamMesh(const uint32_t& modelId, const webifc::geometry::IfcFlatMesh& mesh);
		void getPlacedGeometry(const uint32_t& modelId, const uint32_t& expressId, const webifc::geometry::IfcPlacedGeometry& placedGeometry);
		webifc::geometry::IfcGeometry& getBufferGeometry(const uint32_t& modelId, const webifc::geometry::IfcPlacedGeometry& placedGeometry);
	private:
		int OpenModel(webifc::manager::LoaderSettings& settings,
			const std::function<uint32_t(char*, size_t, size_t)>& requestData = nullptr);
	private:
		std::shared_ptr<webifc::manager::ModelManager> m_ModelManager{ nullptr };
		int m_modelID{ -1 };
		bool m_bMT_ENABLE{ true };
		std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries{};
		std::vector<std::shared_ptr<threepp::Material>> materials{};
	};
}
#endif // !_WEB_IFC_CONVERTER_HPP_