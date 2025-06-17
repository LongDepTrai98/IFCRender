#ifndef _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
#define _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
#include <memory>
#include <vector>
#include <ifcpp/geometry//GeometryConverter.h>
#include <ifcpp/geometry/GeometryInputData.h>
namespace threepp
{
	class Scene; 
}
namespace dragon
{
	class IFCConverter
	{
	public:
		IFCConverter();
		~IFCConverter() = default;
	public:
		void convert(threepp::Scene* scene, const std::shared_ptr<GeometryConverter>& ifcConverter);
		void resolveGeometricItems(std::shared_ptr<ItemShapeData>& geometricItem, carve::math::Matrix& localTransform); 
		void resolveShapeData(shared_ptr<ProductShapeData>& shapeData); 
		std::vector<uint32_t> createIndicesFromMesh(carve::mesh::Mesh<3>* mesh); 
	}; 
}
#endif // !_IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
