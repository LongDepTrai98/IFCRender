#ifndef _ELEMENT_TREE_CTRL_HPP_
#define _ELEMENT_TREE_CTRL_HPP_
#include "wxInclude.hpp"
#include <functional>
#include <vector>
#include <memory>
#include <string>
namespace dragon
{
	class ElementTree;
	class TreeNode;
	class ItemData : public wxTreeItemData
	{
	public:
		typedef void* ItemValue;
	public:
		ItemData(ItemValue data, std::string name) : m_data(data), m_Name(name) {}
		//void ShowInfo(wxTreeCtrl* tree);
		ItemValue GetData() const { return m_data; }
		std::string GetName() const { return m_Name;  }
	private:
		std::string m_Name{ "" }; 
		ItemValue m_data{ nullptr };
	};

	enum class ICON
	{
		ITEM_BIM = 0,
		EXPAND = 1,
		SCENE = 2,
		MAP = 3,
		BIM = 4,
		ITEM_LAYER = 5
	};

	struct func_callback
	{
		std::shared_ptr<std::function<void(const std::pair<int, ItemData*>&)>> m_ToggleStateCallBack{ nullptr };
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_ToggleStateCallBackRecursively{ nullptr };
		std::shared_ptr<std::function<void*(int, std::string)>> m_GetData_Item_Callback{ nullptr };
		ICON icon_item_type;
	};


	class ElementTreeCtrl : public wxTreeCtrl
	{
	public:
		ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style);
		void bindFunc();
	public:
		void setData(std::shared_ptr<ElementTree> treeData, const std::string& key_root);
		void clearData();
		void CreateItemImages();
		void CreateStateImages();
		void OnItemStateClick(wxTreeEvent& event);
		void DoToggleState(const wxTreeItemId& item);
		void DoSetItemState(const wxTreeItemId& item, const int& state);
		void CreateRoot();
	private:
		void RecursiveChildItems(const wxTreeItemId& itemID, const int& parent_item_state, std::vector<std::pair<int, ItemData*>>& itemsData);
		void AddItemsRecursively(const wxTreeItemId& idParent, 
			const std::shared_ptr<TreeNode>& node, 
			const std::string& name);
		void createItemNode(const wxTreeItemId& itemId, std::vector<std::pair<int, ItemData*>>& itemsData);
	public:
		/*std::shared_ptr<std::function<void(const std::pair<int, ItemData*>&)>> m_ToggleStateCallBack{ nullptr };
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_ToggleStateCallBackRecursively{ nullptr };
		std::shared_ptr<std::function<void*(int,std::string)>> m_GetData_Item_Callback{ nullptr };*/
		std::unordered_map<std::string, func_callback> m_umap_callback{}; 
		bool m_bIsItemClickRecursively{ true };
	private:
		std::shared_ptr<ElementTree> m_Tree{ nullptr };
		wxTreeItemId root_item_id;
		wxTreeItemId root_bim_item_id; 
		wxTreeItemId root_map_item_id; 
	};
}
#endif // !_ELEMENT_TREE_CTRL_HPP_