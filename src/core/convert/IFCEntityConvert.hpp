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

		enum class MODE
		{
			MESH,
			INSTANCING
		};

	public:
		IFCConverter();
		~IFCConverter();
	public:
		/*CREATE GROUP MODEL TO EDIT MODE*/
		std::shared_ptr<threepp::Group> convert(const std::shared_ptr<GeometryConverter>& ifcConverter,
			const std::shared_ptr<GeometrySettings>& geom_settings);
		/*CREATE GROUP MODEL TO VIEW MODE*/
		std::shared_ptr<threepp::Group> convertWithInstancing(const std::shared_ptr<GeometryConverter>& ifcConverter,
			const std::shared_ptr<GeometrySettings>& geom_settings);
		void convertProductShape(const std::shared_ptr<ProductShapeData>& product_shape, 
			std::shared_ptr<threepp::Group>& container,
			float transparencyOverride, 
			threepp::Matrix4& parent_matrix);
		void convertGeometricItem(const std::shared_ptr<ItemShapeData>& item_data,
			const std::shared_ptr<IfcProduct>& ifc_product, 
			size_t ii_representation, 
			size_t ii_item,
			std::shared_ptr<threepp::Group>& container,
			float transparencyOverride, 
			threepp::Matrix4& matrix_transform);
		void createOutlineEdgePolygon(std::shared_ptr<threepp::LineSegments>& outlineEdge,
			const std::shared_ptr<threepp::BufferGeometry>& geoBuffer); 
		void createDefaultMaterial(std::shared_ptr<threepp::Material>& material); 
		void convertMeshSets(std::vector<shared_ptr<carve::mesh::MeshSet<3> > >& vecMeshSets,
			std::shared_ptr<threepp::Mesh>& geoMesh,
			size_t ii_item,
			bool disableBackfaceCulling); 
		void convertMeshSetsToBuffGeom(
			std::vector<shared_ptr<carve::mesh::MeshSet<3>>>& vecMeshSets,
			std::shared_ptr<threepp::BufferGeometry>& bufferGeo,
			size_t ii_item,
			bool disableBackfaceCulling
		); 
		void applyStylesToContainer(const std::vector<shared_ptr<StyleData>>& vec_product_styles, 
			std::shared_ptr<threepp::Group>& container,
			std::shared_ptr<threepp::Material>& material,
			float transparencyOverride);
		void createMaterial(const shared_ptr<StyleData>& appearence, 
			float transparencyOverride, 
			std::shared_ptr<threepp::Material>& material);
		void setMaterialForGroup(const std::shared_ptr<threepp::Group>& group, 
			const std::shared_ptr<threepp::Material>& material);
		threepp::Matrix4 convertCarveMatrix2ThreeppMatrix(const carve::math::Matrix& matrix); 
	private: 
		std::shared_ptr<GeometrySettings> m_geomSettings{ nullptr };
		std::unordered_map<std::string, std::shared_ptr<threepp::InstancedMesh>> m_instancing_shape{};
		double m_faces_crease_angle = M_PI * 0.02;
		MODE current_mode{ MODE::MESH }; 
	}; 
}
#endif // !_IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
