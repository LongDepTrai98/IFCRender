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
	class NodeProperties; 
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
		struct IFCNode
		{
			uint32_t expressID{ 0 }; 
			std::vector<std::shared_ptr<IFCNode>> children{}; 
		};

	public:
		IFCProperties(const int& modelID, webifc::manager::ModelManager* model);
		~IFCProperties(); 
	public: 
		void getProperties(); 
		//void getSpatialTreeChunks(const int& modelID); 
		void getChunks(const int& modelID,
			PropsNames prop, 
			std::unordered_map<int,std::vector<int>>& chunk);
		//std::shared_ptr<IFCNode> createTreeNode(const uint32_t& modelID, const std::unordered_map<int, std::vector<int>>& chunk);
		std::shared_ptr<NodeProperties> createTreeNode(const uint32_t& modelID);
	private: 
		webifc::manager::ModelManager* m_modelManager{ nullptr };
		std::map<std::string, PropsNames> m_propsNamesMap;
	};
}
#endif // !_IFC_PROPERTIES_HPP_
