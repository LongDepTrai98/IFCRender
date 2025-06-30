#ifndef _IFC_FILE_CONTEXT_HPP_
#define _IFC_FILE_CONTEXT_HPP_
#include "IFileContext.hpp"
#include <vector>
namespace threepp
{
	class Object3D; 
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
		void handleRaycast(threepp::Raycaster& RayCaster, threepp::Vector2& nor_mouse_pos) override; 
	public: 
		void setRootObject(threepp::Object3D* root_mesh);
	private: 
		std::unique_ptr<IGeometryCache> m_Geometry_Offset_Cache{ nullptr };
		std::vector<threepp::Object3D*> m_Children_Objects{ nullptr }; 
	};
}
#endif // !_IFC_FILE_CONTEXT_HPP_
