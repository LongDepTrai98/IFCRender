#ifndef _IFILE_CONTEXT_HPP_
#define _IFILE_CONTEXT_HPP_
#include <string>
#include <memory>
#include "threepp/math/Vector2.hpp"
namespace threepp
{
	class Raycaster;
	class Mesh; 
}
namespace dragon
{
	class IGeometryCache; 
	class IFileContext
	{
	public: 
		virtual ~IFileContext() {};
	public: 
		virtual std::string getFileType() = 0; 
		virtual IGeometryCache* getGeometryCache() = 0;
		virtual void handleRaycast(threepp::Raycaster& RayCaster, threepp::Vector2& nor_mouse_pos) = 0;
		virtual void handleHoverResult(std::shared_ptr<threepp::Mesh>& object_hover) = 0;
	};
}
#endif // !_IFILE_CONTEXT_HPP_
