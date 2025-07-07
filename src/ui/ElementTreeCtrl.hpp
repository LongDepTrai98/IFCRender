#ifndef _ELEMENT_TREE_CTRL_HPP_
#define _ELEMENT_TREE_CTRL_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class ElementTree;
	class TreeNode; 
	class ElementTreeCtrl : public wxTreeCtrl
	{
	public: 
		ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style);
	public: 
		void setData(std::shared_ptr<ElementTree> treeData);
		void AddItemsRecursively(const wxTreeItemId& idParent, const std::shared_ptr<TreeNode>& node);
		void clearData();
	private: 
		std::shared_ptr<ElementTree> m_Tree{ nullptr };
	};
}
#endif // !_ELEMENT_TREE_CTRL_HPP_
