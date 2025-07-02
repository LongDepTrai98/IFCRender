#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
#include <vector>
namespace threepp
{
	class Object3D;
	class Material;
}
namespace dragon
{
	class IGeometryCache;
	class IFCFileContext : public IFileContext
	{
	public:
		IFCFileContext();
		~IFCFileContext();
	public:
		std::string getFileType() override;
		IGeometryCache* getGeometryCache() override;
		void handleRaycast(CustomRayCaster& RayCaster, threepp::Vector2& nor_mouse_pos) override;
		void handleHoverResult(std::shared_ptr<threepp::Mesh>& object_hover) override;
	public:
		void setRootObject(threepp::Object3D* root_mesh);
	private:
		std::unique_ptr<IGeometryCache> m_Geometry_Offset_Cache{ nullptr };
		std::vector<threepp::Object3D*> m_Children_Objects{ nullptr };
		std::shared_ptr<threepp::Material> m_Material_Hover{ nullptr };
		int m_Current_ExpressID{ -1 };
		int m_Old_ExpressID{ -1 };
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_