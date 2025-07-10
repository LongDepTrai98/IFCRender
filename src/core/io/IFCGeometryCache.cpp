#include "IFCGeometryCache.hpp"
#include "web-ifc/modelmanager/ModelManager.h"
namespace dragon
{
	IFCModelCache::~IFCModelCache()
	{
	}
	void IFCModelCache::clear()
	{
		m_ModelManager->CloseAllModels();
		m_Geometry_Offset.clear();
		m_Object_indices.clear();
		m_Object_Model = nullptr;
		m_ModelManager = nullptr;
	}
	void IFCModelCache::setModelManager(std::shared_ptr<webifc::manager::ModelManager> modelManager, const int& modelID)
	{
		m_modelID = modelID;
		m_ModelManager = modelManager;
	}
}