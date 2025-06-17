#include "IFCMessageHandler.hpp"
#include <ifcpp/IFC4X3/include/IfcBuildingStorey.h>
#include <ifcpp/IFC4X3/include/IfcGloballyUniqueId.h>
#include <ifcpp/IFC4X3/include/IfcLabel.h>
#include <ifcpp/IFC4X3/include/IfcObjectDefinition.h>
#include <ifcpp/IFC4X3/include/IfcProject.h>
#include <ifcpp/IFC4X3/include/IfcRelAggregates.h>
#include <ifcpp/IFC4X3/include/IfcRelContainedInSpatialStructure.h>
#include <ifcpp/IFC4X3/include/IfcText.h>
#include <ifcpp/model/BuildingModel.h>
#include <ifcpp/reader/ReaderSTEP.h>
#include <ifcpp/geometry/GeometryConverter.h>
#include <thread>
namespace dragon
{
	IFCMessageHandler::IFCMessageHandler()
	{
	}
	void IFCMessageHandler::slotMessageWrapper(std::shared_ptr<StatusCallback::Message> message)
	{
		{
			// log file etc can be implemented here
			std::lock_guard<std::mutex> lock(m_MutexMessage);
			StatusCallback::MessageType mType = message->m_message_type;
			if (mType == StatusCallback::MESSAGE_TYPE_PROGRESS_VALUE)
			{
				std::string progressType = message->m_progress_type;

#ifdef _MSC_VER
				int progressChars = 0;
				if (m_MapProcessChars.find(progressType) == m_MapProcessChars.end())
				{
					m_MapProcessChars[progressType] = 0;
				}
				else
				{
					progressChars = m_MapProcessChars[progressType];
				}
				int progressPercent = int(message->m_progress_value * 100);
				for (int ii = 0; ii < progressChars; ++ii)
				{
					std::cout << '\b';
				}
				std::stringstream strs;
				strs << progressPercent << "%";
				m_MapProcessChars[progressType] = strs.str().size();
				std::cout << strs.str();
#endif
			}
		}
	}
}