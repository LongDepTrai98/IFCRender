#include "IFCEntityConvert.hpp"
#include "threepp/threepp.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
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
#include <format>

namespace dragon
{
	IFCConverter::IFCConverter()
	{
	}
	IFCConverter::~IFCConverter()
	{
	}
	std::shared_ptr<threepp::Group> IFCConverter::convert(const std::shared_ptr<GeometryConverter>& geometryConverter,
		const std::shared_ptr<GeometrySettings>& geometrySettings,
		const MODE& open_mode)
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
		float transparency = 1.0f;
		if (shapeDataIfcProject)
		{
			if (open_mode == MODE::MESH)
			{
				convertProductShape(shapeDataIfcProject,
					container,
					transparency);
			}
			else if (open_mode == MODE::INSTANCING)
			{
				std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries{};
				std::vector<std::shared_ptr<threepp::Material>> materials{};
				threepp::Matrix4 parent_matrix{};
				convertProductShape(shapeDataIfcProject,
					transparency,
					parent_matrix,
					geometries,
					materials);
				//Merge Geo
				std::shared_ptr<threepp::Mesh> geoMerge = Mergeo(geometries, materials);
				container->add(geoMerge);
				//Create outline edge
				const float thresholdAngle = 45.0f;
				std::shared_ptr<threepp::EdgesGeometry> edge_geo = threepp::EdgesGeometry::create(*geoMerge->geometry(), thresholdAngle);
				std::shared_ptr<threepp::LineBasicMaterial> outline_material = threepp::LineBasicMaterial::create();
				outline_material->color = threepp::Color::lightslategray;
				std::shared_ptr<threepp::LineSegments> outlineEdge = threepp::LineSegments::create(edge_geo, outline_material);
				container->add(outlineEdge);
			}
		}
		return container;
	}
	void IFCConverter::convertProductShape(const std::shared_ptr<ProductShapeData>& product_shape,
		std::shared_ptr<threepp::Group>& container,
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
			std::shared_ptr<threepp::Group> new_product_group{ nullptr };

			/*UPDATE PARENT MATRIX INSTANCING MODE*/
			new_product_group = threepp::Group::create();
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
					convertGeometricItemEditMode(geom_item,
						ifc_product,
						ii_representation,
						0,
						new_product_group,
						transparencyOverride);
				}
			}
			float transparencyOverrideChildElements = 0.0;
			for (size_t i_item = 0; i_item < product_shape->getChildElements().size(); ++i_item)
			{
				const std::shared_ptr<ProductShapeData>& child = product_shape->getChildElements()[i_item];
				convertProductShape(child,
					new_product_group,
					transparencyOverrideChildElements);
			}

			if (product_shape->getStyles().size() > 0)
			{
				int a = 3;
				//Root
			 //child (matrix ) root matrix * child matrix (mesh )
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

	void IFCConverter::convertProductShape(const std::shared_ptr<ProductShapeData>& product_shape,
		float transparencyOverride,
		threepp::Matrix4& parent_matrix,
		std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries,
		std::vector<std::shared_ptr<threepp::Material>>& materials)
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
			/*UPDATE PARENT MATRIX INSTANCING MODE*/
			threepp_local_transform_matrix = threepp_local_transform_matrix.multiply(parent_matrix);
			product_id = product_shape->m_entity_guid;
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
				else if (dynamic_pointer_cast<IfcCurtainWall>(ifc_product)
					|| dynamic_pointer_cast<IfcWindow>(ifc_product))
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
							if (dynamic_pointer_cast<IfcCurtainWall>(relating_object)
								|| dynamic_pointer_cast<IfcWindow>(relating_object))
							{
								transparencyOverride = 0.6f;
							}
						}
					}
				}

				for (size_t ii_representation = 0; ii_representation < product_shape->getGeometricItems().size(); ++ii_representation)
				{
					const shared_ptr<ItemShapeData>& geom_item = product_shape->getGeometricItems()[ii_representation];
					convertGeometricItemViewMode(geom_item,
						ifc_product,
						ii_representation,
						0,
						transparencyOverride,
						threepp_local_transform_matrix,
						geometries,
						materials);
				}
			}
			float transparencyOverrideChildElements = 0.0;
			for (size_t i_item = 0; i_item < product_shape->getChildElements().size(); ++i_item)
			{
				const std::shared_ptr<ProductShapeData>& child = product_shape->getChildElements()[i_item];
				convertProductShape(child,
					transparencyOverrideChildElements,
					threepp_local_transform_matrix,
					geometries,
					materials);
			}

			if (product_shape->getStyles().size() > 0)
			{
				int a = 3;
				//Root
			 //child (matrix ) root matrix * child matrix (mesh )
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

	void IFCConverter::convertGeometricItemEditMode(const std::shared_ptr<ItemShapeData>& item_data,
		const std::shared_ptr<IfcProduct>& ifc_product,
		size_t ii_representation, size_t ii_item,
		std::shared_ptr<threepp::Group>& container,
		float transparencyOverride)
	{
		/*CONVERT GEOMETRIC ITEMS EDIT MODE*/
		bool includeChildProducts = false;
		bool includeGeometricChildItems = false;
		bool isEnableBackFaceCulling = true;
		if (item_data->hasItemDataGeometricRepresentation(includeGeometricChildItems, true))
		{
			std::string product_guid;
			if (ifc_product->m_GlobalId)
			{
				product_guid = ifc_product->m_GlobalId->m_value;
			}
			std::string ifc_entity_type = EntityFactory::getStringForClassID(ifc_product->classID());
			std::shared_ptr<threepp::Mesh> meshGeo{ nullptr };
			std::shared_ptr<threepp::BufferGeometry> buffGeo{ nullptr };
			std::shared_ptr<threepp::LineSegments> outlineEdge{ nullptr };
			std::shared_ptr<threepp::Material> material{ nullptr };
			if (item_data->m_meshsets_open.size() > 0)
			{
				// disable back face culling for open meshes
				convertMeshSetsToBuffGeom(item_data->m_meshsets_open,
					buffGeo,
					ii_item,
					true);
			}
			else if (item_data->m_meshsets.size() > 0)
			{
				// enable back face culling for open meshes
				convertMeshSetsToBuffGeom(item_data->m_meshsets,
					buffGeo,
					ii_item,
					false);
				isEnableBackFaceCulling = false;
			}
			if (item_data->getStyles().size() > 0)
			{
				createMaterialWithStyle(item_data->getStyles(),
					material,
					transparencyOverride);
			}

			if (buffGeo)
			{
				createOutlineEdgePolygon(outlineEdge, buffGeo);
				if (!material) createDefaultMaterial(material);
				isEnableBackFaceCulling ? material->side = threepp::Side::Front : material->side = threepp::Side::Double;
				meshGeo = threepp::Mesh::create(buffGeo, material);
				container->add(meshGeo);
				container->add(outlineEdge);
			}
		}

		for (size_t i_item = 0; i_item < item_data->m_child_items.size(); ++i_item)
		{
			const shared_ptr<ItemShapeData>& child = item_data->m_child_items[i_item];
			convertGeometricItemEditMode(child,
				ifc_product,
				ii_representation,
				i_item,
				container,
				transparencyOverride);
		}
	}
	void IFCConverter::convertGeometricItemViewMode(const std::shared_ptr<ItemShapeData>& item_data,
		const std::shared_ptr<IfcProduct>& ifc_product,
		size_t ii_representation, size_t ii_item,
		float transparencyOverride,
		threepp::Matrix4& matrix_transform,
		std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries,
		std::vector<std::shared_ptr<threepp::Material>>& materials)
	{
		/*CONVERT GEOMTRIC ITEM VIEW MODE*/
		bool includeChildProducts = false;
		bool includeGeometricChildItems = false;
		bool isEnableBackFaceCulling = true;
		if (item_data->hasItemDataGeometricRepresentation(includeGeometricChildItems, true))
		{
			std::string product_guid;
			if (ifc_product->m_GlobalId)
			{
				product_guid = ifc_product->m_GlobalId->m_value;
			}
			std::string ifc_entity_type = EntityFactory::getStringForClassID(ifc_product->classID());
			std::shared_ptr<threepp::BufferGeometry> buffGeo{ nullptr };
			std::shared_ptr<threepp::Material> material{ nullptr };
			/*INSTANCING THINKING*/
			if (item_data->m_meshsets_open.size() > 0)
			{
				// disable back face culling for open meshes
				convertMeshSetsToBuffGeom(item_data->m_meshsets_open,
					buffGeo,
					ii_item,
					true);
			}
			else if (item_data->m_meshsets.size() > 0)
			{
				// enable back face culling for open meshes
				convertMeshSetsToBuffGeom(item_data->m_meshsets,
					buffGeo,
					ii_item,
					false);
				isEnableBackFaceCulling = false;
			}

			if (item_data->getStyles().size() > 0)
			{
				createMaterialWithStyle(item_data->getStyles(),
					material,
					transparencyOverride);
			}

			if (buffGeo)
			{
				buffGeo->applyMatrix4(matrix_transform);
				if (!material)createDefaultMaterial(material);
				isEnableBackFaceCulling ? material->side = threepp::Side::Front : material->side = threepp::Side::Double;
				geometries.emplace_back(buffGeo);
				materials.emplace_back(material);
			}
		}

		for (size_t i_item = 0; i_item < item_data->m_child_items.size(); ++i_item)
		{
			const shared_ptr<ItemShapeData>& child = item_data->m_child_items[i_item];
			convertGeometricItemViewMode(child,
				ifc_product,
				ii_representation,
				i_item,
				transparencyOverride,
				matrix_transform,
				geometries,
				materials);
		}
	}

	void IFCConverter::createOutlineEdgePolygon(std::shared_ptr<threepp::LineSegments>& outlineEdge,
		const std::shared_ptr<threepp::BufferGeometry>& geoBuffer)
	{
		//create edge geometry
		const float thresholdAngle = 30.0f;
		std::shared_ptr<threepp::EdgesGeometry> edge_geo = threepp::EdgesGeometry::create(*geoBuffer, thresholdAngle);
		if (!outlineEdge)
		{
			std::shared_ptr<threepp::LineBasicMaterial> outline_material = threepp::LineBasicMaterial::create();
			outline_material->color = threepp::Color::darkgray;
			outlineEdge = threepp::LineSegments::create(edge_geo, outline_material);
		}
	}

	void IFCConverter::createDefaultMaterial(std::shared_ptr<threepp::Material>& material)
	{
		material = threepp::MeshBasicMaterial::create();
		material->as<threepp::MeshBasicMaterial>()->color = threepp::Color::lightgray;
	}

	void IFCConverter::createMaterialWithStyle(const std::vector<shared_ptr<StyleData>>& vec_product_styles,
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
			if (style->m_apply_to_geometry_type == StyleData::GEOM_TYPE_SURFACE
				|| style->m_apply_to_geometry_type == StyleData::GEOM_TYPE_ANY)
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
		const float w_ambient = 0.35f;
		const float w_diffuse = 0.65f;
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
		float mix_r = w_ambient * color_ambient_r + w_diffuse * color_diffuse_r;
		float mix_g = w_ambient * color_ambient_g + w_diffuse * color_diffuse_g;
		float mix_b = w_ambient * color_ambient_b + w_diffuse * color_diffuse_b;
		float mix_a = w_ambient * color_ambient_a + w_diffuse * color_diffuse_a;
		float alpha = 1.f;
		if (transparencyOverride > 0.0f)
		{
			set_transparent = true;
			alpha = transparencyOverride;
		}
		else if (transparency > 0.0f)	// transparency: 0 = opaque, 1 = fully transparent
		{
			set_transparent = true;
			alpha = transparency;
		}
		material = threepp::MeshBasicMaterial::create();
		//material->as<threepp::MeshBasicMaterial>()->vertexColors = true;
		material->as<threepp::MeshBasicMaterial>()->color = threepp::Color(mix_r, mix_g, mix_b);
		material->transparent = set_transparent;
		material->polygonOffset = true;
		material->polygonOffsetFactor = 1.0f;
		material->polygonOffsetUnits = 1.0f;
		material->opacity = alpha;
		material->side = threepp::Side::Front;
		material->needsUpdate();
	}

	void IFCConverter::setMaterialForGroup(const std::shared_ptr<threepp::Group>& group, const std::shared_ptr<threepp::Material>& material)
	{
	}

	void IFCConverter::convertMeshSetsToBuffGeom(std::vector<shared_ptr<carve::mesh::MeshSet<3>>>& vecMeshSets,
		std::shared_ptr<threepp::BufferGeometry>& geom,
		size_t ii_item,
		bool disableBackfaceCulling)
	{
		double min_triangle_area = m_geomSettings->getMinTriangleArea();
		double eps = m_geomSettings->getEpsilonMergePoints();
		double crease_angle = m_faces_crease_angle;
		size_t mesh_size = vecMeshSets.size();
		std::vector<std::shared_ptr<threepp::BufferGeometry>> buffGeometries{};
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
				vertices.insert(vertices.end(), { static_cast<float>(point.x),static_cast<float>(point.y),static_cast<float>(point.z) });
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
			buffGeometries.emplace_back(bufferGeo);
		}
		/*MERGE GEO*/
		if (buffGeometries.size() != 1)
		{
			std::cout << std::format("buff size {}", buffGeometries.size()) << std::endl;
			geom = threepp::mergeBufferGeometries(buffGeometries);
		}
		else if (buffGeometries.size() == 1) {
			geom = buffGeometries[0];
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
	std::shared_ptr<threepp::Mesh> IFCConverter::Mergeo(const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries, const std::vector<std::shared_ptr<threepp::Material>>& materials)
	{
		std::cout << "Merging Phase:" << "----------------------------------------------" << std::endl;
		std::shared_ptr<threepp::BufferGeometry> buffMerge = threepp::mergeBufferGeometries(geometries);
		std::string mess{};
		int32_t indexOffset{ 0 };
		for (int i = 0; i < materials.size(); ++i)
		{
			int index_count = geometries[i]->getIndex()->count();
			buffMerge->addGroup(indexOffset, index_count, i);
			std::stringstream strs;
			for (int ii = 0; ii < mess.size(); ++ii)
			{
				std::cout << '\b';
			}
			mess = std::format("Merge geo {}, total {}", i + 1, materials.size());
			strs << mess;
			std::cout << strs.str();
			indexOffset += index_count;
		}
		std::cout << std::endl;
		std::cout << "Merged Done:" << "----------------------------------------------" << std::endl;
		return threepp::Mesh::create(buffMerge, materials);
	}
}