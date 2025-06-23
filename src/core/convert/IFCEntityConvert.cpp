#include "IFCEntityConvert.hpp"
#include "threepp/threepp.hpp"
#include "threepp/geometries/EdgesGeometry.hpp"
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
#include <ifcpp/model/BasicTypes.h>
#include <ifcpp/model/StatusCallback.h>
#include <ifcpp/IFC4X3/EntityFactory.h>
#include <ifcpp/IFC4X3/include/IfcCurtainWall.h>
#include <ifcpp/IFC4X3/include/IfcFeatureElementSubtraction.h>
#include <ifcpp/IFC4X3/include/IfcOpeningElement.h>
#include <ifcpp/IFC4X3/include/IfcPropertySetDefinitionSet.h>
#include <ifcpp/IFC4X3/include/IfcSpace.h>
#include <ifcpp/IFC4X3/include/IfcWindow.h>
#include <ifcpp/geometry/GeometrySettings.h>
#include <ifcpp/geometry/MeshOps.h>

namespace dragon
{
	IFCConverter::IFCConverter()
	{
	}
	std::shared_ptr<threepp::Group> IFCConverter::convert(const std::shared_ptr<GeometryConverter>& geometryConverter, 
		const std::shared_ptr<GeometrySettings>& geometrySettings)
	{
		/*CREAT GROUP TO STORE ENTITY*/
		std::shared_ptr<threepp::Group> container = threepp::Group::create(); 
		std::unordered_map<std::string, std::shared_ptr<ProductShapeData>>& map_shape_data = geometryConverter->getShapeInputData();
		m_geomSettings = geometrySettings; 
		shared_ptr<ProductShapeData> shapeDataIfcProject;
		for (auto& it : map_shape_data)
		{
			std::shared_ptr<ProductShapeData> shapeData = it.second;
			if (!shapeData)
			{
				continue; 
			}
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
			//resolveShapeData(shapeDataIfcProject, container);
			convertProductShape(shapeDataIfcProject,container,1.0f);
		}
		return container; 
	}
	void IFCConverter::convertProductShape(const std::shared_ptr<ProductShapeData>& product_shape, std::shared_ptr<threepp::Group>& container, 
		float transparencyOverride)
	{
		std::string product_id{ "" }; 
		try
		{
			product_shape->m_added_to_spatial_structure = true; 
			std::string entityType{ "" }; 
			if (product_shape->m_ifc_object_definition.expired())
			{
				return; 
			}

			/*GET PARENT MATRIX*/
			carve::math::Matrix localTransform = product_shape->getTransform();
			threepp::Matrix4 threepp_local_transform_matrix = convertCarveMatrix2ThreeppMatrix(localTransform);
			std::shared_ptr<threepp::Group> new_product_group = threepp::Group::create();
			new_product_group->applyMatrix4(threepp_local_transform_matrix);
			container->add(new_product_group);
			product_id = product_shape->m_entity_guid;
			new_product_group->name = product_id;


			std::shared_ptr<IfcObjectDefinition> ifc_object_def(product_shape->m_ifc_object_definition);
			entityType = EntityFactory::getStringForClassID(ifc_object_def->classID());

			std::shared_ptr<IfcProduct> ifc_product = dynamic_pointer_cast<IfcProduct>(ifc_object_def);
			if (ifc_product)
			{
				// enable transparency for certain objects
				if (dynamic_pointer_cast<IfcSpace>(ifc_product))
				{
					transparencyOverride = 0.1f;
				}
				else if (dynamic_pointer_cast<IfcCurtainWall>(ifc_product) || dynamic_pointer_cast<IfcWindow>(ifc_product))
				{
					transparencyOverride = 0.2f;
				}
				// check if parent building element is window
				if (ifc_product->m_Decomposes_inverse.size() > 0)
				{
					for (size_t ii_decomposes = 0; ii_decomposes < ifc_product->m_Decomposes_inverse.size(); ++ii_decomposes)
					{
						const weak_ptr<IfcRelAggregates>& decomposes_weak = ifc_product->m_Decomposes_inverse[ii_decomposes];
						if (decomposes_weak.expired())
						{
							continue;
						}
						shared_ptr<IfcRelAggregates> decomposes_ptr(decomposes_weak);
						shared_ptr<IfcObjectDefinition>& relating_object = decomposes_ptr->m_RelatingObject;
						if (relating_object)
						{
							if (dynamic_pointer_cast<IfcCurtainWall>(relating_object) || dynamic_pointer_cast<IfcWindow>(relating_object))
							{
								transparencyOverride = 0.6f;
							}
						}
					}
				}

				for (size_t ii_representation = 0; ii_representation < product_shape->getGeometricItems().size(); ++ii_representation)
				{
					const shared_ptr<ItemShapeData>& geom_item = product_shape->getGeometricItems()[ii_representation];
					convertGeometricItem(geom_item, ifc_product, ii_representation, 0, new_product_group, transparencyOverride); 
				}
			}
			float transparencyOverrideChildElements = 0.0;
			for (size_t i_item = 0; i_item < product_shape->getChildElements().size(); ++i_item)
			{
				const std::shared_ptr<ProductShapeData>& child = product_shape->getChildElements()[i_item];
				convertProductShape(child, new_product_group, transparencyOverrideChildElements);
			}

			if (product_shape->getStyles().size() > 0)
			{
				int a = 3; 
			}


		}
		catch (BuildingException& e)
		{
			std::cerr << e.what() << std::endl; 
		}
		catch (carve::exception& e)
		{
			std::cerr << e.str() << std::endl; 
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl; 
		}
	}

	void IFCConverter::convertGeometricItem(const std::shared_ptr<ItemShapeData>& item_data, 
		const std::shared_ptr<IfcProduct>& ifc_product, 
		size_t ii_representation, size_t ii_item,
		std::shared_ptr<threepp::Group>& container, 
		float transparencyOverride)
	{
		bool includeChildProducts = false;
		bool includeGeometricChildItems = false;
		if (item_data->hasItemDataGeometricRepresentation(includeGeometricChildItems, true))
		{

			std::string product_guid;
			std::shared_ptr<threepp::Mesh> meshGeo{ nullptr }; 
			std::shared_ptr<threepp::LineSegments> outlineEdge{ nullptr }; 
			if (ifc_product->m_GlobalId)
			{
				product_guid = ifc_product->m_GlobalId->m_value;
			}

			std::string ifc_entity_type = EntityFactory::getStringForClassID(ifc_product->classID());
			std::shared_ptr<threepp::Material> material{ nullptr }; 
			if (item_data->getStyles().size() > 0)
			{
				applyStylesToContainer(item_data->getStyles(),
					container,
					material,
					transparencyOverride);
			}

			if (item_data->m_meshsets_open.size() > 0)
			{
				// disable back face culling for open meshes
				convertMeshSets(item_data->m_meshsets_open, meshGeo, ii_item, true);
			}

			if (item_data->m_meshsets.size() > 0)
			{
				// enable back face culling for open meshes 
				convertMeshSets(item_data->m_meshsets, meshGeo, ii_item, false);
			}

			if (meshGeo)
			{
				meshGeo->name = product_guid;
				//create outline of geometry 
				createOutlineEdgePolygon(outlineEdge,
					meshGeo->geometry()); 
				if (!material)
				{
					createDefaultMaterial(material); 
				}
				meshGeo->setMaterial(material);
				container->add(meshGeo); 
				container->add(outlineEdge); 
			}

		}

		for (size_t i_item = 0; i_item < item_data->m_child_items.size(); ++i_item)
		{
			const shared_ptr<ItemShapeData>& child = item_data->m_child_items[i_item];
			convertGeometricItem(child, ifc_product, ii_representation, i_item, container, transparencyOverride);
		}
	}

	void IFCConverter::createOutlineEdgePolygon(std::shared_ptr<threepp::LineSegments>& outlineEdge,
		const std::shared_ptr<threepp::BufferGeometry>& geoBuffer)
	{
		//create edge geometry 
		const float thresholdAngle = 15.0f; 
		std::shared_ptr<threepp::EdgesGeometry> edge_geo = threepp::EdgesGeometry::create(*geoBuffer, thresholdAngle);
		if (!outlineEdge)
		{
			std::shared_ptr<threepp::LineBasicMaterial> outline_material = threepp::LineBasicMaterial::create(); 
			outline_material->color = threepp::Color::darkgray; 
			outline_material->linewidth = 0.5f; 
			outlineEdge = threepp::LineSegments::create(edge_geo,outline_material);
			outlineEdge->renderOrder = 1; 
		}
	}

	void IFCConverter::createDefaultMaterial(std::shared_ptr<threepp::Material>& material)
	{
		material = threepp::MeshLambertMaterial::create();
		material->as<threepp::MeshLambertMaterial>()->color = threepp::Color::gray; 
	}

	void IFCConverter::applyStylesToContainer(const std::vector<shared_ptr<StyleData>>& vec_product_styles, 
		std::shared_ptr<threepp::Group>& container, 
		std::shared_ptr<threepp::Material>& material,
		float transparencyOverride)
	{
		for (size_t ii = 0; ii < vec_product_styles.size(); ++ii)
		{
			const shared_ptr<StyleData>& style = vec_product_styles[ii];
			if (!style)
			{
				continue;
			}
			if (style->m_apply_to_geometry_type == StyleData::GEOM_TYPE_SURFACE || style->m_apply_to_geometry_type == StyleData::GEOM_TYPE_ANY)
			{
				createMaterial(style,
					transparencyOverride,
					material); 
			}
		}
	}

	void IFCConverter::createMaterial(const shared_ptr<StyleData>& appearence,
		float transparencyOverride, 
		std::shared_ptr<threepp::Material>& material)
	{
		if (!appearence)
		{
			return;
		}
		float shininess = appearence->m_shininess;
		float transparency = appearence->m_transparency;
		bool set_transparent = false;

		const float color_ambient_r = appearence->m_color_ambient.r;
		const float color_ambient_g = appearence->m_color_ambient.g;
		const float color_ambient_b = appearence->m_color_ambient.b;
		const float color_ambient_a = appearence->m_color_ambient.a;

		const float color_diffuse_r = appearence->m_color_diffuse.r;
		const float color_diffuse_g = appearence->m_color_diffuse.g;
		const float color_diffuse_b = appearence->m_color_diffuse.b;
		const float color_diffuse_a = appearence->m_color_diffuse.a;

		const float color_specular_r = appearence->m_color_specular.r;
		const float color_specular_g = appearence->m_color_specular.g;
		const float color_specular_b = appearence->m_color_specular.b;
		const float color_specular_a = appearence->m_color_specular.a;

		if (transparencyOverride > 0)
		{
			set_transparent = true;
			transparency = transparencyOverride;
		}

		float alpha = 1.f;
		if (transparency > 0.01)	// transparency: 0 = opaque, 1 = fully transparent
		{
			set_transparent = true;
			alpha = 1.0f - transparency;
		}
		material = threepp::MeshPhongMaterial::create();
		material->as<threepp::MeshPhongMaterial>()->color = threepp::Color(color_diffuse_r, color_diffuse_g, color_diffuse_b);
		material->as<threepp::MeshPhongMaterial>()->specular = threepp::Color(color_specular_r, color_specular_g, color_specular_b);
		material->as<threepp::MeshPhongMaterial>()->shininess = shininess;
		material->transparent = set_transparent; 
		material->polygonOffset = true; 
		material->polygonOffsetFactor = 1.0f;
		material->polygonOffsetUnits = 1.0f; 
		material->needsUpdate(); 
		material->opacity = alpha; 
	}

	void IFCConverter::convertMeshSets(std::vector<shared_ptr<carve::mesh::MeshSet<3>>>& vecMeshSets,
		std::shared_ptr<threepp::Mesh>& geoMesh,
		size_t ii_item,
		bool disableBackfaceCulling)
	{
		double min_triangle_area = m_geomSettings->getMinTriangleArea();
		double eps = m_geomSettings->getEpsilonMergePoints();
		double crease_angle = m_faces_crease_angle;
		size_t mesh_size = vecMeshSets.size(); 
		for (size_t ii = 0; ii < vecMeshSets.size(); ++ii)
		{
			//Create buffer geometry 
			std::vector<float> vertices;
			std::vector<float> normals;
			std::vector<int> indices;
			shared_ptr<carve::mesh::MeshSet<3>>& item_meshset = vecMeshSets[ii];
			double epsCoplanarFacesAngle = eps;
			double minFaceArea = eps;
			bool dumpMeshes = false;
			PolyInputCache3D polyTriangulated;
			GeomProcessingParams params(m_geomSettings, dumpMeshes);
			params.epsMergePoints = 0.001;

			MeshOps::retriangulateMeshSetForExport(item_meshset, polyTriangulated, params);
			if (!polyTriangulated.m_poly_data)
			{
				return;
			}
			//create new buffer geometry 
			std::shared_ptr<threepp::BufferGeometry> bufferGeo = threepp::BufferGeometry::create(); 
			for (const vec3& point : polyTriangulated.m_poly_data->points)
			{
				vertices.insert(vertices.end(), {static_cast<float>(point.x),static_cast<float>(point.y),static_cast<float>(point.z) });
				normals.insert(normals.end(), { static_cast<float>(point.x),static_cast<float>(point.y),static_cast<float>(point.z) });
			}; 
			for (auto it = polyTriangulated.m_poly_data->faceIndices.begin(); it != polyTriangulated.m_poly_data->faceIndices.end(); ++it) {
				int numPoints = *it;
				if (numPoints != 3) {
					std::cout << "not triangularized" << std::endl;
					continue;
				}
				++it;
				int idx = *it;
				++it;
				indices.push_back(static_cast<int>(idx));
				idx = *it;
				++it;
				indices.push_back(static_cast<int>(idx));
				idx = *it;
				indices.push_back(static_cast<int>(idx));
			}
			bufferGeo->setIndex(indices);
			bufferGeo->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
			bufferGeo->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
			geoMesh = threepp::Mesh::create(bufferGeo);
			geoMesh->castShadow = true; 
		}
	}

	
































	std::shared_ptr<threepp::Group> IFCConverter::resolveGeometricItems(std::shared_ptr<ItemShapeData>& geometricItem,
		std::shared_ptr<threepp::Group>& container)
	{
		std::shared_ptr<threepp::Group> group = threepp::Group::create(); 
		/*ADD GROUP TO CONTAINER*/
		container->add(group); 
		// closed meshes
		for (auto meshset : geometricItem->m_meshsets)
		{
			std::vector<carve::mesh::Vertex<3> >& vertexData = meshset->vertex_storage;
			for (auto mesh : meshset->meshes)
			{
				/*CREATE MESH HERE*/
				std::shared_ptr<threepp::BufferGeometry> buffer_mesh =  createBufferGeometryFromCarveMesh(mesh); 
				/*MATERIAL FOR MESH*/
				auto material = threepp::MeshPhongMaterial::create();
				auto lst_style = geometricItem->getStyles(); 
				for (auto style : lst_style)
				{
					auto color_ambient = style->m_color_ambient;
					auto color_diffuse = style->m_color_diffuse;
					auto color_specular = style->m_color_specular;
					material->color = threepp::Color(color_diffuse.r, color_diffuse.g, color_diffuse.b); 
					material->specular = threepp::Color(color_specular.r, color_specular.g, color_specular.b);
					material->shininess = style->m_shininess; 
					material->transparent = style->m_transparency; 
				}
				std::shared_ptr<threepp::Mesh> threepp_mesh = threepp::Mesh::create(buffer_mesh, material); 
				group->add(threepp_mesh);
			}
		}
		// open meshes
		for (auto meshset : geometricItem->m_meshsets_open)
		{
			for (auto mesh : meshset->meshes)
			{
				/*CREATE MESH HERE*/
				std::shared_ptr<threepp::BufferGeometry> buffer_mesh = createBufferGeometryFromCarveMesh(mesh);
				/*MATERIAL FOR MESH*/
				auto material = threepp::MeshPhongMaterial::create();
				auto lst_style = geometricItem->getStyles();
				for (auto style : lst_style)
				{
					auto color_ambient = style->m_color_ambient;
					auto color_diffuse = style->m_color_diffuse;
					auto color_specular = style->m_color_specular;
					material->color = threepp::Color(color_diffuse.r, color_diffuse.g, color_diffuse.b);
					material->specular = threepp::Color(color_specular.r, color_specular.g, color_specular.b);
					material->shininess = style->m_shininess;
					material->transparent = style->m_transparency;
				}
				std::shared_ptr<threepp::Mesh> threepp_mesh = threepp::Mesh::create(buffer_mesh, material);
				group->add(threepp_mesh);
			}
		}
		// traverse geometry
		for (auto childItem : geometricItem->m_child_items)
		{
			resolveGeometricItems(childItem, container);
		}
		return group; 
	}
	void IFCConverter::resolveShapeData(shared_ptr<ProductShapeData>& shapeData,
		std::shared_ptr<threepp::Group>& container)
	{
		//shape container 
		std::shared_ptr<threepp::Group> shape_container = threepp::Group::create();
		/*ADD SHAPE CONTAINER TO GROUP*/
		container->add(shape_container); 
		/*APPLY MATRIX TO SHAPE CONTAINER*/
		carve::math::Matrix localTransform = shapeData->getTransform();
		threepp::Matrix4 threepp_local_transform_matrix = convertCarveMatrix2ThreeppMatrix(localTransform); 
		shape_container->applyMatrix4(threepp_local_transform_matrix); 
		// traverse geometry
		for (auto geometricItem : shapeData->getGeometricItems())
		{
			// geometric items can have child items too
			resolveGeometricItems(geometricItem, shape_container);
		}

		for (auto child_object : shapeData->getChildElements())
		{
			// child elements in case of IfcBuildingStorey, IfcElementAssembly etc.
			resolveShapeData(child_object, shape_container);
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
		std::array<float, 16> arr_matrix = {
			(float)matrix._11,(float)matrix._12,(float)matrix._13,(float)matrix._14,
			(float)matrix._21,(float)matrix._22,(float)matrix._23,(float)matrix._24,
			(float)matrix._31,(float)matrix._32,(float)matrix._33,(float)matrix._34,
			(float)matrix._41,(float)matrix._42,(float)matrix._43,(float)matrix._44
		}; 
		return threepp::Matrix4(arr_matrix);
	}
	std::shared_ptr<threepp::BufferGeometry> IFCConverter::createBufferGeometryFromCarveMesh(carve::mesh::Mesh<3>* mesh)
	{
		size_t faces = mesh->faces.size();
		std::shared_ptr<threepp::BufferGeometry> geometry = std::make_shared<threepp::BufferGeometry>(); 
		size_t vertices_size = faces * 3 * 3; 
		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<int> indices; 
		int index{ 0 }; 
		std::map<carve::mesh::Vertex<3>*, int> vertex_map;//map to create lst indices 
		/*CREATE BUFFER GEOMETRY WITH LIST POINT IN FACE*/
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
					vertices.insert(vertices.end(), { (float)pointLocal[0],(float)pointLocal[1],(float)pointLocal[2] }); 
					normals.insert(normals.end(), { (float)pointLocal[0],(float)pointLocal[1],(float)pointLocal[2] }); 
					vertex_map.insert({ current_edge->vert,index });
					++index; 
				}
				/*PUSHBACK INDEX*/
				indices.push_back(vertex_map[current_edge->vert]);
				current_edge = current_edge->next;
			} 
			while (current_edge != start_edge);
		}
		geometry->setIndex(indices); 
		geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3)); 
		geometry->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
		return geometry; 
	}
}