#ifndef _IFC_PROPERTIES_HPP_
#define _IFC_PROPERTIES_HPP_
#include <string>
#include <vector>
namespace webifc::manager
{
	class ModelManager; 
}
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
	public:
		IFCProperties(webifc::manager::ModelManager* model);
		~IFCProperties(); 
	public: 
		void getProperties(); 
	private: 
		webifc::manager::ModelManager* m_modelManager{ nullptr }; 
	};
}
#endif // !_IFC_PROPERTIES_HPP_
