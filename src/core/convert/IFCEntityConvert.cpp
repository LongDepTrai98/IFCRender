#include "IFCEntityConvert.hpp"
#include "threepp/threepp.hpp"
#include <unordered_set>
#include <ifcpp/IFC4X3/include/IfcBuildingStorey.h>
#include <ifcpp/IFC4X3/include/IfcGloballyUniqueId.h>
#include <ifcpp/IFC4X3/include/IfcLabel.h>
#include <ifcpp/IFC4X3/include/IfcObjectDefinition.h>
#include <ifcpp/IFC4X3/include/IfcProject.h>
#include <ifcpp/IFC4X3/include/IfcRelAggregates.h>
#include <ifcpp/IFC4X3/include/IfcRelContainedInSpatialStructure.h>
#include <ifcpp/IFC4X3/include/IfcText.h>
#include <ifcpp/model/BuildingModel.h>
#include <ifcpp/reader/ReaderSTEP.h>
#include <ifcpp/geometry/GeometryConverter.h>

namespace dragon
{
	IFCConverter::IFCConverter()
	{
	}
	std::shared_ptr<threepp::Group> IFCConverter::convert(const std::shared_ptr<GeometryConverter>& geometryConverter)
	{
		/*CREAT GROUP TO STORE ENTITY*/
		std::shared_ptr<threepp::Group> container = threepp::Group::create(); 
		std::unordered_map<std::string, std::shared_ptr<ProductShapeData>>& map_entities = geometryConverter->getShapeInputData();
		shared_ptr<ProductShapeData> shapeDataIfcProject;
		for (auto& it : map_entities)
		{
			std::shared_ptr<ProductShapeData> shapeData = it.second;
			if (shapeData->m_ifc_object_definition.expired())
			{
				continue;
			}
			shared_ptr<IfcObjectDefinition> ifcObject = shared_ptr<IfcObjectDefinition>(shapeData->m_ifc_object_definition);

			// check for certain type of the entity:
			shared_ptr<IfcProject> ifc_project = dynamic_pointer_cast<IfcProject>(ifcObject);
			if (ifc_project)
			{
				shapeDataIfcProject = shapeData;
				break;
			}
		}
		if (shapeDataIfcProject)
		{
			resolveShapeData(shapeDataIfcProject, container);
		}
		return container; 
	}
	std::shared_ptr<threepp::Group> IFCConverter::resolveGeometricItems(std::shared_ptr<ItemShapeData>& geometricItem, carve::math::Matrix& localTransform)
	{
		// closed meshes
		for (auto meshset : geometricItem->m_meshsets)
		{
			std::vector<carve::mesh::Vertex<3> >& vertexData = meshset->vertex_storage;
			for (auto mesh : meshset->meshes)
			{
				std::vector<uint32_t> indices;
				std::vector<carve::geom::vector<3>> lstVertex;
				createIndicesAndVertexFromMesh(mesh, localTransform, indices, lstVertex);
			}
		}
		// open meshes
		for (auto meshset : geometricItem->m_meshsets_open)
		{
			std::vector<carve::mesh::Vertex<3> >& vertexData = meshset->vertex_storage;
			for (auto mesh : meshset->meshes)
			{
				std::vector<uint32_t> indices;
				std::vector<carve::geom::vector<3>> lstVertex;
				createIndicesAndVertexFromMesh(mesh,localTransform,indices,lstVertex);
			}
		}
		// traverse geometry
		for (auto childItem : geometricItem->m_child_items)
		{
			resolveGeometricItems(childItem, localTransform);
		}
		return nullptr; 
	}
	void IFCConverter::resolveShapeData(shared_ptr<ProductShapeData>& shapeData, std::shared_ptr<threepp::Group>& container)
	{
		//shape container 
		std::shared_ptr<threepp::Group> shape_container = threepp::Group::create(); 
		carve::math::Matrix localTransform = shapeData->getTransform();
		//apply localTransform cho group
		threepp::Matrix4 threepp_local_transform_matrix = convertCarveMatrix2ThreeppMatrix(localTransform); 
		shape_container->applyMatrix4(threepp_local_transform_matrix); 
		// traverse geometry
		for (auto geometricItem : shapeData->getGeometricItems())
		{
			// geometric items can have child items too
			resolveGeometricItems(geometricItem, localTransform);
		}

		for (auto child_object : shapeData->getChildElements())
		{
			// child elements in case of IfcBuildingStorey, IfcElementAssembly etc.
			resolveShapeData(child_object, container);
		}
	}
	void IFCConverter::createIndicesAndVertexFromMesh(carve::mesh::Mesh<3>* mesh,
		carve::math::Matrix& localTransform,
		std::vector<uint32_t>& indices,
		std::vector<carve::geom::vector<3>>& lstVertex)
	{
		size_t face_size = mesh->faces.size();
		std::map<carve::mesh::Vertex<3>*,int> vertex_map;
		for (auto face : mesh->faces)
		{
			carve::mesh::Edge<3>* edge = face->edge;
			edge = edge->next;
			size_t edge_size = face->n_edges;
			auto* start_edge = face->edge;
			auto* current_edge = start_edge;
			do {
				carve::mesh::Vertex<3>* vertex = current_edge->vert;
				if (vertex_map.count(vertex) == 0)
				{
					carve::geom::vector<3> pointLocal = vertex->v;
					carve::geom::vector<3> pointGlobal = localTransform * pointLocal;
					lstVertex.push_back(pointGlobal);
					uint32_t vertex_idx = lstVertex.size() - 1; 
					vertex_map.insert({ current_edge->vert,vertex_idx});
				}
				/*PUSHBACK INDEX*/
				indices.push_back(vertex_map[current_edge->vert]);
				current_edge = current_edge->next;
			} while (current_edge != start_edge);
		}
		if (indices.size() % 3 != 0) {
			throw std::exception("Warning: Indices count not divisible by 3 (may not form complete triangles)"); 
		}
	}
	threepp::Matrix4 IFCConverter::convertCarveMatrix2ThreeppMatrix(const carve::math::Matrix& matrix)
	{
		return threepp::Matrix4();
	}
}