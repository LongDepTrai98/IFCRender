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
}
namespace dragon
{
	class IFCConverter
	{
	public:
		IFCConverter();
		~IFCConverter() = default;
	public:
		std::shared_ptr<threepp::Group> convert(const std::shared_ptr<GeometryConverter>& ifcConverter);
		std::shared_ptr<threepp::Group> resolveGeometricItems(std::shared_ptr<ItemShapeData>& geometricItem, carve::math::Matrix& localTransform); 
		void resolveShapeData(shared_ptr<ProductShapeData>& shapeData,std::shared_ptr<threepp::Group>& container); 
		void createIndicesAndVertexFromMesh(carve::mesh::Mesh<3>* mesh, carve::math::Matrix& localTransform, std::vector<uint32_t>& indices, std::vector<carve::geom::vector<3>>& lstVertex);
		threepp::Matrix4 convertCarveMatrix2ThreeppMatrix(const carve::math::Matrix& matrix); 
	}; 
}
#endif // !_IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
