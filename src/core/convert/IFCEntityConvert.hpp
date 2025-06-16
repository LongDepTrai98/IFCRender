#ifndef _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
#define _IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
namespace threepp
{
	class Scene; 
}
class GeometryConverter; 
namespace dragon
{
	class IFCConverter
	{
	public:
		IFCConverter();
		~IFCConverter() = default;
	public:
		void convert(threepp::Scene* scene, GeometryConverter* ifcConverter);
	}; 
}
#endif // !_IFC_ENTITY_CONVERT_TO_SCENE_OBJECT_HPP_
