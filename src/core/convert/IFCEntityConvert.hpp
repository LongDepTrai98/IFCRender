#ifndef _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
#define _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
#include <memory>
#include <vector>
#include <ifcpp/geometry//GeometryConverter.h>
#include <ifcpp/geometry/GeometryInputData.h>
#include <threepp/math/Matrix4.hpp>
#include <unordered_set>
namespace threepp
{
	class Scene;
	class Group; 
	class Mesh; 
	class BufferGeometry;
	class Material; 
	class LineSegments; 
	class InstancedMesh; 
}
namespace dragon
{
	class IFCConverter
	{
	public: 
		enum class MODE
		{
			MESH,
			INSTANCING
		};
		struct BuffCache {
			std::shared_ptr<threepp::Material> material{ nullptr }; 
			std::shared_ptr<threepp::BufferGeometry> buffer{ nullptr }; 
		};
		/*GEO CONTAIN MATERIAL AND BUFFER GEOMETRY*/
		struct Geometry {
			std::shared_ptr<threepp::BufferGeometry> geometry{ nullptr }; 
			std::shared_ptr<threepp::Material> material{ nullptr }; 
		};
		struct Instance_Cache {
			/*GEO TO MERGE*/
			std::vector<BuffCache> lstBuffGeo{};
		};
	public:
		IFCConverter();
		~IFCConverter();
	public:
		/*CREATE GROUP MODEL TO EDIT MODE*/
		std::shared_ptr<threepp::Group> convert(const std::shared_ptr<GeometryConverter>& ifcConverter,
			const std::shared_ptr<GeometrySettings>& geom_settings, 
			const MODE& open_mode = MODE::MESH);
		/*CREATE GROUP MODEL TO VIEW MODE*/
		void convertProductShape(const std::shared_ptr<ProductShapeData>& product_shape, 
			std::shared_ptr<threepp::Group>& container,
			float transparencyOverride);
		void convertProductShape(const std::shared_ptr<ProductShapeData>& product_shape,
			float transparencyOverride,
			threepp::Matrix4& parent_matrix,
			std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries,
			std::vector<std::shared_ptr<threepp::Material>>& materials); 
		/*EDIT MODE*/
		void convertGeometricItemEditMode(const std::shared_ptr<ItemShapeData>& item_data,
			const std::shared_ptr<IfcProduct>& ifc_product, 
			size_t ii_representation, 
			size_t ii_item,
			std::shared_ptr<threepp::Group>& container,
			float transparencyOverride);
		/*MERGEO MODE*/
		void convertGeometricItemViewMode(const std::shared_ptr<ItemShapeData>& item_data,
			const std::shared_ptr<IfcProduct>& ifc_product,
			size_t ii_representation,
			size_t ii_item, 
			float transparencyOverride,
			threepp::Matrix4& matrix_transform,
			std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries,
			std::vector<std::shared_ptr<threepp::Material>>& materials);
		void createOutlineEdgePolygon(std::shared_ptr<threepp::LineSegments>& outlineEdge,
			const std::shared_ptr<threepp::BufferGeometry>& geoBuffer); 
		void createDefaultMaterial(std::shared_ptr<threepp::Material>& material); 
		void convertMeshSetsToBuffGeom(
			std::vector<shared_ptr<carve::mesh::MeshSet<3>>>& vecMeshSets,
			std::shared_ptr<threepp::BufferGeometry>& bufferGeo,
			size_t ii_item,
			bool disableBackfaceCulling
		); 
		void createMaterialWithStyle(const std::vector<shared_ptr<StyleData>>& vec_product_styles, 
			std::shared_ptr<threepp::Material>& material,
			float transparencyOverride);
		void createMaterial(const shared_ptr<StyleData>& appearence, 
			float transparencyOverride, 
			std::shared_ptr<threepp::Material>& material);
		void setMaterialForGroup(const std::shared_ptr<threepp::Group>& group, 
			const std::shared_ptr<threepp::Material>& material);
		threepp::Matrix4 convertCarveMatrix2ThreeppMatrix(const carve::math::Matrix& matrix); 
		std::shared_ptr<threepp::Mesh> Mergeo(const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries,
			const std::vector<std::shared_ptr<threepp::Material>>& materials);
	private: 
		std::shared_ptr<GeometrySettings> m_geomSettings{ nullptr };
		double m_faces_crease_angle = M_PI * 0.02;
		MODE m_current_mode{ MODE::MESH };
	}; 
}
#endif // !_IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
