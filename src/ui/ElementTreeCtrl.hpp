#ifndef _ELEMENT_TREE_CTRL_HPP_
#define _ELEMENT_TREE_CTRL_HPP_
#include "wxInclude.hpp"
#include <functional>
namespace dragon
{
	class ElementTree;
	class TreeNode;
	class ItemData : public wxTreeItemData
	{
	public:
		ItemData(int* data_id) : m_data_id(data_id) {}
		//void ShowInfo(wxTreeCtrl* tree);
		int* GetData() const { return m_data_id; }

	private:
		int* m_data_id{ 0 };
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
		void RecursiveChildItems(const wxTreeItemId& itemID,const int& parent_item_state);
		void AddItemsRecursively(const wxTreeItemId& idParent, const std::shared_ptr<TreeNode>& node);
	public: 
		std::function<void(const wxTreeItemId&)> toggleStateCallBack{ nullptr }; 
		bool m_bIsItemClickRecursively{ true };
	private:
		std::shared_ptr<ElementTree> m_Tree{ nullptr };
	};
}
#endif // !_ELEMENT_TREE_CTRL_HPP_