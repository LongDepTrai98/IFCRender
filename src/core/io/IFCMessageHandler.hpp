#ifndef _IFC_MESSAGE_HANDLER_HPP_
#define _IFC_MESSAGE_HANDLER_HPP_
#include <mutex>
#include <memory>
#include <map>
#include <ifcpp/model/StatusCallback.h>
namespace dragon
{
	class IFCMessageHandler
	{
	public: 
		IFCMessageHandler(); 
	public: 
		void slotMessageWrapper(std::shared_ptr<StatusCallback::Message> message); 
	private: 
		std::map<std::string, int> m_MapProcessChars;
		std::mutex m_MutexMessage{}; 
	};
}
#endif // !_IFC_MESSAGE_HANDLER_HPP_
