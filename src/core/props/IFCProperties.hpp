#ifndef _IFC_PROPERTIES_HPP_
#define _IFC_PROPERTIES_HPP_
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include "core/utils/WebIFCHelper.hpp"
namespace dragon
{
	class ElementTree;
	class IFCProperties
	{
	public:
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
		void getChunks(const int& modelID,
			PropsNames prop,
			std::unordered_map<int, std::vector<int>>& chunk);
		std::shared_ptr<ElementTree> createTreeNode(const uint32_t& modelID);
	private:
		webifc::manager::ModelManager* m_modelManager{ nullptr };
		std::map<std::string, PropsNames> m_propsNamesMap;
	};
}
#endif // !_IFC_PROPERTIES_HPP_