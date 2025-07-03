#define FMT_UNICODE 0
#include "WebIFCConverter.hpp"
#include "web-ifc/parsing/IfcLoader.h"
#include "web-ifc/schema/IfcSchemaManager.h"
#include "web-ifc/schema/ifc-schema.h"
#include "web-ifc/modelmanager/ModelManager.h"
#include "web-ifc/geometry/IfcGeometryProcessor.h"
#include "io_helpers.h"
#include "threepp/threepp.hpp"
#include "threepp/geometries/EdgesGeometry.hpp"
#include "threepp/materials/ShaderMaterial.hpp"
#include "core/utils/StringHelper.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "core/utils/MathHelper.hpp"

namespace dragon
{
	WebIFCConverter::WebIFCConverter()
	{
		m_ModelManager = std::make_shared<webifc::manager::ModelManager>(m_bMT_ENABLE);
	}

	WebIFCConverter::~WebIFCConverter()
	{
		m_ModelManager->CloseModel(m_modelID);
		geo_with_material.clear();
	}

	std::shared_ptr<threepp::Group> WebIFCConverter::convert(const std::filesystem::path& path)
	{
		const std::string& buffer = StringHelper::ReadFile(path.string());
		parseIfcFile(buffer);
		if (!m_ModelManager->IsModelOpen(m_modelID))
		{
			spdlog::error(std::format("Model not open {}", path.string()));
		}
		auto model_size = m_ModelManager->GetIfcLoader(m_modelID)->GetTotalSize();
		std::shared_ptr<threepp::Group> container = threepp::Group::create();
		std::array<float, 16> default_array = container->matrix->elements;
		std::array<double, 16> double_array{};
		MathHelper::convertFloatArr2DoubleArr(default_array, double_array);
		m_ModelManager->GetGeometryProcessor(m_modelID)->SetTransformation(double_array);
		loadAllGeometry(m_modelID);
		auto size_t = geo_with_material.size();
		std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries{};
		std::vector<std::shared_ptr<threepp::Material>> materials{};
		for (auto& [hashColor, geo_with_mat] : geo_with_material)
		{
			std::shared_ptr<threepp::BufferGeometry> merged = threepp::mergeBufferGeometries(geo_with_mat.geometries);
			geometries.emplace_back(merged);
			materials.emplace_back(geo_with_mat.material);
		}
		std::shared_ptr<threepp::BufferGeometry> merged_all = threepp::mergeBufferGeometries(geometries, true);
		std::shared_ptr<threepp::Mesh> mesh = threepp::Mesh::create(merged_all, materials);
		mesh->matrixAutoUpdate = false;
		container->add(mesh);
		const float thresholdAngle = 30.0f;
		std::shared_ptr<threepp::EdgesGeometry> edge_geo = threepp::EdgesGeometry::create(*mesh->geometry(), thresholdAngle);
		std::shared_ptr<threepp::LineBasicMaterial> outline_material = threepp::LineBasicMaterial::create();
		outline_material->color = threepp::Color::darkgray;
		std::shared_ptr<threepp::LineSegments> outlineEdge = threepp::LineSegments::create(edge_geo, outline_material);
		container->add(outlineEdge);
		geometries.clear();
		materials.clear();
		return container;
	}

	void WebIFCConverter::parseIfcFile(const std::string& buffer)
	{
		//call back to load part of file data
		const std::function<uint32_t(char*, size_t, size_t)> request_data = [&](char* dest, size_t sourceOffset, size_t destSize) {
			uint32_t length = std::min(buffer.size() - sourceOffset, destSize);
			memcpy(dest, &buffer[sourceOffset], length);
			std::cout << std::format("load part sourceOffSet {}, destSize {}", sourceOffset, destSize);
			return length;
			};
		webifc::manager::LoaderSettings settings = webifc::manager::LoaderSettings();
		settings.COORDINATE_TO_ORIGIN = true;
		m_modelID = this->OpenModel(settings, request_data);
	}

	void WebIFCConverter::loadAllGeometry(const uint32_t& modelID)
	{
		/*INIT FOR UI*/
		streamAllMeshes(modelID);
	}

	void WebIFCConverter::streamAllMeshes(const uint32_t& modelID)
	{
		if (!m_ModelManager->IsModelOpen(modelID)) return;
		std::vector<uint32_t> types{};
		const std::unordered_set<uint32_t>& ifcElementList = m_ModelManager->GetSchemaManager().GetIfcElementList();
		for (auto& type : ifcElementList)
		{
			if (type == webifc::schema::IFCOPENINGELEMENT
				|| type == webifc::schema::IFCSPACE
				|| type == webifc::schema::IFCOPENINGSTANDARDCASE)
			{
				continue;
			}
			types.push_back(type);
		}
		streamAllMeshesWithTypes(modelID, types);
	}

	void WebIFCConverter::streamMeshes(const uint32_t& modelId, const std::vector<uint32_t>& expressIds)
	{
		if (!m_ModelManager->IsModelOpen(modelId)) return;
		auto geomLoader = m_ModelManager->GetGeometryProcessor(modelId);
		int index = 0;
		int total = expressIds.size();
		for (const auto& id : expressIds)
		{
			/*READ MESH FROM IFC FILE*/
			webifc::geometry::IfcFlatMesh mesh = geomLoader->GetFlatMesh(id);
			for (auto& geom : mesh.geometries)
			{
				auto& flatGeom = geomLoader->GetGeometry(geom.geometryExpressID);
				flatGeom.GetVertexData();
			}
			if (!mesh.geometries.empty())
			{
				streamMesh(modelId, mesh);
			}
		}
		geomLoader->Clear();
		index++;
	}

	void WebIFCConverter::streamMesh(const uint32_t& modelId, const webifc::geometry::IfcFlatMesh& mesh)
	{
		const std::vector<webifc::geometry::IfcPlacedGeometry>& placedGeometries = mesh.geometries;
		for (int i = 0; i < placedGeometries.size(); ++i)
		{
			auto& placedGeometry = placedGeometries[i];
			getPlacedGeometry(modelId, mesh.expressID, placedGeometry);
		}
	}

	void WebIFCConverter::getPlacedGeometry(const uint32_t& modelId, const uint32_t& expressId, const webifc::geometry::IfcPlacedGeometry& placedGeometry)
	{
		auto& geometry = getBufferGeometry(modelId, placedGeometry);
		auto vertexData = geometry.vertexData;
		auto indices = geometry.indexData;
		int vertices_size = vertexData.size() / 2;
		int normals_size = vertexData.size() / 2;
		int attribute_size = vertexData.size() / 6;
		std::vector<float> vertices(vertices_size);
		std::vector<float> normals(normals_size);
		std::vector<uint32_t> idAttribute(attribute_size);
		//IGeometryCache::offset geometry_offset{};
		bool isFirstPoint{ false };
		for (int i = 0; i < vertexData.size(); i += 6)
		{
			/*POINT X*/
			vertices[i / 2] = vertexData[i];
			index_offset++;
			if (!isFirstPoint)
			{
				//geometry_offset.begin = index_offset;
				isFirstPoint = true;
			}
			/*POINT Y*/
			vertices[i / 2 + 1] = vertexData[i + 1];
			index_offset++;
			/*POINT Z*/
			vertices[i / 2 + 2] = vertexData[i + 2];
			index_offset++;
			/*NORMAL POINT*/
			normals[i / 2] = vertexData[i + 3];
			normals[i / 2 + 1] = vertexData[i + 4];
			normals[i / 2 + 2] = vertexData[i + 5];
			idAttribute[i / 6] = expressId;
		}
		//geometry_offset.end = index_offset;
		auto placedColor = placedGeometry.color;
		std::string str_hash_color = MathHelper::colorToHash(placedGeometry.color.x, placedColor.y, placedColor.z, placedColor.w);
		if (geo_with_material.count(str_hash_color) == 0)
		{
			/*CREATE MATERIAL*/
			std::shared_ptr<threepp::MeshLambertMaterial> material = threepp::MeshLambertMaterial::create();
			threepp::Color color;
			color.setRGB(placedColor.x, placedColor.y, placedColor.z);
			std::cout << std::format("Hash color : {}", str_hash_color) << std::endl;
			material->as<threepp::MeshLambertMaterial>()->color = color;
			material->side = threepp::Side::Double;
			material->transparent = placedColor.w != 1.0;
			if (material->transparent) material->opacity = placedColor.w;
			std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries;
			geo_with_material.insert({ str_hash_color,{material,geometries} });
		}
		std::shared_ptr<threepp::BufferGeometry> buff_geometry = threepp::BufferGeometry::create();
		buff_geometry->setIndex(indices);
		buff_geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		buff_geometry->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
		buff_geometry->setAttribute("expressID", threepp::IntBufferAttribute::create(idAttribute, 1));
		std::array<float, 16> matrix_float{};
		MathHelper::convertDoubleArr2FloatArr(placedGeometry.flatTransformation, matrix_float);
		//geometry_offset.geometry = buff_geometry;
		//m_Geometry_Offset[expressId].emplace_back(geometry_offset);
		buff_geometry->applyMatrix4(threepp::Matrix4(matrix_float));
		geo_with_material[str_hash_color].geometries.emplace_back(buff_geometry);
	}

	webifc::geometry::IfcGeometry& WebIFCConverter::getBufferGeometry(const uint32_t& modelId, const webifc::geometry::IfcPlacedGeometry& placedGeometry)
	{
		if (m_ModelManager->IsModelOpen(modelId));
		auto geomLoader = m_ModelManager->GetGeometryProcessor(modelId);
		return geomLoader->GetGeometry(placedGeometry.geometryExpressID);
	}

	std::shared_ptr<webifc::manager::ModelManager> WebIFCConverter::getModelManager()
	{
		return m_ModelManager;
	}

	const int& WebIFCConverter::getModelId() const
	{
		return m_modelID;
	}

	//const std::map<int, std::vector<IGeometryCache::offset>>& WebIFCConverter::getGeometryOffsetCache()
	//{
	//	return m_Geometry_Offset;
	//}

	void WebIFCConverter::streamAllMeshesWithTypes(const uint32_t& modelID, const std::vector<uint32_t>& types)
	{
		if (!m_ModelManager->IsModelOpen(modelID)) return;
		auto loader = m_ModelManager->GetIfcLoader(modelID);
		for (auto& type : types)
		{
			auto elements = loader->GetExpressIDsWithType(type);
			if (elements.size() != 0)
			{
				streamMeshes(modelID, elements);
			}
		}
	}

	int WebIFCConverter::OpenModel(webifc::manager::LoaderSettings& settings,
		const std::function<uint32_t(char*, size_t, size_t)>& requestData)
	{
		if (!m_ModelManager) return -1;
		const int& modelID = m_ModelManager->CreateModel(settings);
		if (requestData)
		{
			m_ModelManager->GetIfcLoader(modelID)->LoadFile(requestData);
		}
		return modelID;
	}
}