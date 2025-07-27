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
#include "core/utils/MathHelper.hpp"
#include "config/app_config.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <execution>
#include <algorithm>
namespace dragon
{
	WebIFCConverter::WebIFCConverter()
	{
		m_ModelManager = std::make_shared<webifc::manager::ModelManager>(m_bMT_ENABLE);
	}

	WebIFCConverter::~WebIFCConverter()
	{
		geo_with_material.clear();
	}

	std::shared_ptr<threepp::Group> WebIFCConverter::convert(const std::filesystem::path& path)
	{
		std::string buffer;
		StringHelper::readFile(path.string(), buffer);
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
		std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries{};
		std::vector<std::shared_ptr<threepp::Material>> materials{};
		int material_index{ 0 };
		for (auto& [hashColor, geo_with_mat] : geo_with_material)
		{
			/*CREATE DUNG INDEX*/
			std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries_in_mat{};
			for (auto& geometry_with_expressID : geo_with_mat.geometries)
			{
				index_offset++;
				vertex_offset++;
				auto begin_offset_index = index_offset;
				auto begin_offset_vertex = vertex_offset;
				uint32_t expressID = geometry_with_expressID.expressID;
				geometries_in_mat.emplace_back(geometry_with_expressID.geometry);
				size_t geometry_index_size = geometries_in_mat.back()->getIndex()->array().size();
				index_offset += geometry_index_size - 1;
				//position
				auto geometry_vertex_size = geometries_in_mat.back()->getAttribute<float>("position")->array().size();
				vertex_offset += geometry_vertex_size - 1;
				auto end_offset_index = index_offset;
				auto end_offset_vertex = vertex_offset;
				spdlog::info("geo with expressID {} , begin index offset {} , end index offset {}", expressID, begin_offset_index, end_offset_index);
				spdlog::info("geo with expressID {} , begin vertex offset {} , end vertex offset {}", expressID, begin_offset_vertex, end_offset_vertex);
				spdlog::info("geo with expressID {} , material index {}", expressID, material_index);
				if (geometry_offset_with_expressID.find(expressID) == geometry_offset_with_expressID.end())
				{
					IFCModelCache::element element;
					element.state = 1;
					element.totalIndices = 0;
					element.totalVertices = 0;
					geometry_offset_with_expressID.insert({ expressID,element });
				}
				geometry_offset_with_expressID[expressID].offsets.push_back({ begin_offset_vertex,
					end_offset_vertex,
					begin_offset_index,
					end_offset_index,
					material_index,
					1
					});
				geometry_offset_with_expressID[expressID].totalIndices += end_offset_index - begin_offset_index + 1;
				geometry_offset_with_expressID[expressID].totalVertices += end_offset_vertex - begin_offset_vertex + 1;
			}
			std::shared_ptr<threepp::BufferGeometry> merged = threepp::mergeBufferGeometries(geometries_in_mat);
			geometries.emplace_back(merged);
			materials.emplace_back(geo_with_mat.material);
			material_index++;
		}
		/*SORT INDIECES*/
		std::shared_ptr<threepp::BufferGeometry> merged_all = threepp::mergeBufferGeometries(geometries, true);
		if (index_offset != merged_all->getIndex()->array().size() - 1)
		{
			spdlog::error("index_offset diff mer_array_indices - 1 : {}, {}", index_offset, merged_all->getIndex()->array().size() - 1);
		}
		if (vertex_offset != merged_all->getAttribute<float>("position")->array().size() - 1)
		{
			spdlog::error("vertex_offset diff mer_vertex_offset - 1 : {}, {}", vertex_offset, merged_all->getAttribute<float>("position")->array().size() - 1);
		}

		/*CREATE MESH*/
		std::shared_ptr<threepp::Mesh> mesh = threepp::Mesh::create(merged_all, materials);
		mesh->name = "model";
		mesh->matrixAutoUpdate = false;
		container->add(mesh);
		/*CREATE OUTLINE EDGE*/
		geometries.clear();
		materials.clear();
		m_ModelManager->clearGeometryLoader(m_modelID);
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
				|| type == webifc::schema::IFCSPACE)
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
		spdlog::info("Create geo from placedGeometry with expressID {}", expressId);
		if (!m_ModelManager->IsModelOpen(modelId)) {
			throw std::runtime_error("Model is not open");
		}
		auto& geometry = getBufferGeometry(modelId, placedGeometry);
		auto vertexData = geometry.vertexData;
		auto indices = geometry.indexData;
		int vertices_size = vertexData.size() / 2;
		int normals_size = vertexData.size() / 2;
		int attribute_size = vertexData.size() / 6;
		std::vector<float> vertices(vertices_size);
		vertices.reserve(vertices_size);
		std::vector<float> normals(normals_size);
		normals.reserve(normals_size);
		std::vector<uint32_t> idAttribute(attribute_size);
		idAttribute.reserve(attribute_size);
		for (int i = 0; i < vertexData.size(); i += 6)
		{
			/*POINT X*/
			vertices[i / 2] = vertexData[i];
			/*POINT Y*/
			vertices[i / 2 + 1] = vertexData[i + 1];
			//index_offset++;
			/*POINT Z*/
			vertices[i / 2 + 2] = vertexData[i + 2];
			/*NORMAL POINT*/
			normals[i / 2] = vertexData[i + 3];
			normals[i / 2 + 1] = vertexData[i + 4];
			normals[i / 2 + 2] = vertexData[i + 5];
			idAttribute[i / 6] = expressId;
		}
		auto placedColor = placedGeometry.color;
		std::string str_hash_color = MathHelper::colorToHash(placedGeometry.color.x, placedColor.y, placedColor.z, placedColor.w);
		if (geo_with_material.count(str_hash_color) == 0)
		{
			/*CREATE MATERIAL*/
			std::shared_ptr<threepp::MeshPhongMaterial> material = threepp::MeshPhongMaterial::create();
			threepp::Color color;
			color.setRGB(placedColor.x, placedColor.y, placedColor.z);
			material->shininess = 100.0f;
			material->color = color;
			material->side = threepp::Side::Double;
			material->transparent = placedColor.w != 1.0;
			if (material->transparent) material->opacity = placedColor.w;
			std::vector<GeoWithExpressID> geometriesWithExpressID;
			geo_with_material.insert({ str_hash_color,{material,geometriesWithExpressID} });
		}
		std::shared_ptr<threepp::BufferGeometry> buff_geometry = threepp::BufferGeometry::create();
		buff_geometry->setIndex(indices);
		buff_geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		buff_geometry->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
		buff_geometry->setAttribute("expressID", threepp::IntBufferAttribute::create(idAttribute, 1));
		std::array<float, 16> matrix_float{};
		MathHelper::convertDoubleArr2FloatArr(placedGeometry.flatTransformation, matrix_float);
		buff_geometry->applyMatrix4(threepp::Matrix4(matrix_float));
		geo_with_material[str_hash_color].geometries.push_back({ buff_geometry ,expressId });
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

	std::unordered_map<unsigned int, IFCModelCache::element>& WebIFCConverter::getGeometryOffset()
	{
		return geometry_offset_with_expressID;
	}

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