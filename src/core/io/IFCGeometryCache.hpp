#ifndef _IFC_GEOMETRY_CACHE_HPP_
#define _IFC_GEOMETRY_CACHE_HPP_
#include "IGeometryCache.hpp"
#include <map>
#include <vector>
namespace webifc::manager {
	class ModelManager;
}
namespace dragon
{
	class IFCGeometryCache : public IGeometryCache
	{
	public:
		~IFCGeometryCache();
		IFCGeometryCache() = default;
	public:
		void clear() override;
		void setModelManager(std::shared_ptr<webifc::manager::ModelManager> modelManager, const int& modelID);
	private:
		std::shared_ptr<webifc::manager::ModelManager> m_ModelManager{ nullptr };
		int m_modelID{ -1 };
	};
}
#endif // !_IFC_GEOMETRY_CACHE_HPP_