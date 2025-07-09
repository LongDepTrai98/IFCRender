#ifndef _ELEMENT_TREE_CTRL_HPP_
#define _ELEMENT_TREE_CTRL_HPP_
#include "wxInclude.hpp"
#include <functional>
#include <vector>
#include <memory>
#include <variant>
namespace dragon
{
	class ElementTree;
	class TreeNode;
	class ItemData : public wxTreeItemData
	{
		using ItemValue = std::variant<int*>; 
	public:
		ItemData(ItemValue data) : m_data(data) {}
		//void ShowInfo(wxTreeCtrl* tree);
		ItemValue GetData() const { return m_data; }
	private:
		ItemValue m_data{ nullptr };
	};

	class ElementTreeCtrl : public wxTreeCtrl
	{
	public:
		ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style);
		void bindFunc(); 
	public:
		void setData(std::shared_ptr<ElementTree> treeData);
		void clearData();
		void CreateStateImages();
		void OnItemStateClick(wxTreeEvent& event);
		void DoToggleState(const wxTreeItemId& item);
		void DoSetItemState(const wxTreeItemId& item, const int& state); 
	private: 
		void RecursiveChildItems(const wxTreeItemId& itemID,const int& parent_item_state, std::vector<std::pair<int, ItemData*>>& itemsData);
		void AddItemsRecursively(const wxTreeItemId& idParent, const std::shared_ptr<TreeNode>& node);
	public: 
		std::shared_ptr<std::function<void(const std::pair<int,ItemData*>&)>> m_ToggleStateCallBack{ nullptr }; 
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_ToggleStateCallBackRecursively{ nullptr };
		bool m_bIsItemClickRecursively{ true };
	private:
		std::shared_ptr<ElementTree> m_Tree{ nullptr };
	};
}
#endif // !_ELEMENT_TREE_CTRL_HPP_