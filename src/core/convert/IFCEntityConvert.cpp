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
	void IFCConverter::convert(threepp::Scene* scene, const std::shared_ptr<GeometryConverter>& geometryConverter)
	{
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
			resolveShapeData(shapeDataIfcProject);
		}
	}
	void IFCConverter::resolveGeometricItems(std::shared_ptr<ItemShapeData>& geometricItem, carve::math::Matrix& localTransform)
	{
		// closed meshes
		for (auto meshset : geometricItem->m_meshsets)
		{
			std::vector<carve::mesh::Vertex<3> >& vertexData = meshset->vertex_storage;
			for (auto mesh : meshset->meshes)
			{
				for (auto face : mesh->faces)
				{
					carve::mesh::Edge<3>* edge = face->edge;
					for (size_t ii = 0; ii < face->n_edges; ++ii)
					{
						carve::mesh::Vertex<3>* vertex = edge->vert;
						carve::geom::vector<3> pointLocal = vertex->v;
						carve::geom::vector<3> pointGlobal = localTransform * pointLocal;
						double x = pointGlobal.x;
						double y = pointGlobal.y;
						double z = pointGlobal.z;
						std::cout << "point in mesh: (" << x << "/" << y << "/" << z << ")" << std::endl;
					}
				}
			}
		}

		// open meshes
		for (auto meshset : geometricItem->m_meshsets_open)
		{
			std::vector<carve::mesh::Vertex<3> >& vertexData = meshset->vertex_storage;
			for (auto mesh : meshset->meshes)
			{
				for (auto face : mesh->faces)
				{
					carve::mesh::Edge<3>* edge = face->edge;
					for (size_t ii = 0; ii < face->n_edges; ++ii)
					{
						carve::mesh::Vertex<3>* vertex = edge->vert;
						carve::geom::vector<3> pointLocal = vertex->v;
						carve::geom::vector<3> pointGlobal = localTransform * pointLocal;
						double x = pointGlobal.x;
						double y = pointGlobal.y;
						double z = pointGlobal.z;
						std::cout << "point in mesh: (" << x << "/" << y << "/" << z << ")" << std::endl;
					}
				}
			}
		}

		// traverse geometry
		for (auto childItem : geometricItem->m_child_items)
		{
			resolveGeometricItems(childItem, localTransform);
		}
	}
	void IFCConverter::resolveShapeData(shared_ptr<ProductShapeData>& shapeData)
	{
		carve::math::Matrix localTransform = shapeData->getTransform();

		// traverse geometry
		for (auto geometricItem : shapeData->getGeometricItems())
		{
			// geometric items can have child items too
			resolveGeometricItems(geometricItem, localTransform);
		}

		for (auto child_object : shapeData->getChildElements())
		{
			// child elements in case of IfcBuildingStorey, IfcElementAssembly etc.
			resolveShapeData(child_object);
		}
	}
}