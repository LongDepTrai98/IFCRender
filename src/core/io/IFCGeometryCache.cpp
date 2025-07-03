#include "IFCGeometryCache.hpp"
#include "web-ifc/modelmanager/ModelManager.h"
namespace dragon
{
	IFCGeometryCache::~IFCGeometryCache()
	{
		m_ModelManager->CloseAllModels();
	}
	void IFCGeometryCache::clear()
	{
		m_ModelManager->CloseAllModels();
	}
	void IFCGeometryCache::setModelManager(std::shared_ptr<webifc::manager::ModelManager> modelManager, const int& modelID)
	{
		m_modelID = modelID;
		m_ModelManager = modelManager;
	}
}