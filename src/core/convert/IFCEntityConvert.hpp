#ifndef _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
#define _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
#include <memory>
#include <vector>
#include <ifcpp/geometry//GeometryConverter.h>
#include <ifcpp/geometry/GeometryInputData.h>
#include <threepp/math/Matrix4.hpp>
namespace threepp
{
	class Scene;
	class Group; 
	class Mesh; 
	class BufferGeometry;
}
namespace dragon
{
	class IFCConverter
	{
	public:
		IFCConverter();
		~IFCConverter() = default;
	public:
		std::shared_ptr<threepp::Group> convert(const std::shared_ptr<GeometryConverter>& ifcConverter, const std::shared_ptr<GeometrySettings>& geom_settings);
		void convertProductShape(const std::shared_ptr<ProductShapeData>& product_shape, std::shared_ptr<threepp::Group>& container, float transparencyOverride);
		void convertGeometricItem(const std::shared_ptr<ItemShapeData>& item_data, const std::shared_ptr<IfcProduct>& ifc_product, size_t ii_representation, size_t ii_item,
			std::shared_ptr<threepp::Group>& parentNode, float transparencyOverride);
		std::shared_ptr<threepp::Group> resolveGeometricItems(std::shared_ptr<ItemShapeData>& geometricItem, std::shared_ptr<threepp::Group>& container);
		void resolveShapeData(shared_ptr<ProductShapeData>& shapeData,std::shared_ptr<threepp::Group>& container); 
		void createIndicesAndVertexFromMesh(carve::mesh::Mesh<3>* mesh, carve::math::Matrix& localTransform, std::vector<uint32_t>& indices, std::vector<carve::geom::vector<3>>& lstVertex);
		threepp::Matrix4 convertCarveMatrix2ThreeppMatrix(const carve::math::Matrix& matrix); 
		std::shared_ptr<threepp::BufferGeometry> createBufferGeometryFromCarveMesh(carve::mesh::Mesh<3>* mesh); 
	}; 
}
#endif // !_IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
