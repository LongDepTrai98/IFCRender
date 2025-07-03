#include "IFCProperties.hpp"
#include "core/utils/WebIFCHelper.hpp"

namespace dragon
{
	IFCProperties::IFCProperties(webifc::manager::ModelManager* model) : m_modelManager(model)
	{
	}
	IFCProperties::~IFCProperties()
	{
		m_modelManager = nullptr;
	}
	void IFCProperties::getProperties()
	{
	}
}