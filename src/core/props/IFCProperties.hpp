#ifndef _IFC_PROPERTIES_HPP_
#define _IFC_PROPERTIES_HPP_
#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include "core/utils/WebIFCHelper.hpp"
namespace dragon
{
	class IFCProperties
	{
	public: 
		struct Node
		{
			int expressID{ -1 };
			std::string type{ "" };
			std::vector<Node*> children{ nullptr }; 
		};
		struct PropsNames
		{
			uint32_t expressID{ 0 };
			std::string relating{ "" };
			std::string related{ "" }; 
			std::string key{ "" };
		};
	public:
		IFCProperties(const int& modelID, webifc::manager::ModelManager* model);
		~IFCProperties(); 
	public: 
		void getProperties(); 
		void getSpatialTreeChunks(const int& modelID); 
		void getChunks(const int& modelID, PropsNames prop);
	private: 
		webifc::manager::ModelManager* m_modelManager{ nullptr };
		std::map<std::string, PropsNames> m_propsNamesMap;
	};
}
#endif // !_IFC_PROPERTIES_HPP_
