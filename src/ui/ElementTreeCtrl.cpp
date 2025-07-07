#include "ElementTreeCtrl.hpp"
#include "core/node/ElementTree.hpp"
namespace dragon
{
	ElementTreeCtrl::ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style)
		: wxTreeCtrl(parent,
			wxID_ANY,
			postion,
			size,
			style)
	{
	}
	void ElementTreeCtrl::setData(std::shared_ptr<ElementTree> treeData)
	{
		clearData(); 
		m_Tree = treeData; 
		auto parent = m_Tree->m_Parent; 
		wxString str;
		str.Printf(parent->getLabelNode().c_str()); 
		wxTreeItemId root_id = this->AddRoot(str);
		AddItemsRecursively(root_id, parent); 
		this->ExpandAll(); 
	}
	void ElementTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent, const std::shared_ptr<TreeNode>& node)
	{
		wxString str;
		for (auto& childNode : node->children)
		{
			str.Printf(childNode->getLabelNode().c_str());
			wxTreeItemId id_item = AppendItem(idParent,str,-1,-1, new ItemData(&childNode->getData()));
			AddItemsRecursively(id_item, childNode); 
		}
	}
	void ElementTreeCtrl::clearData()
	{
		m_Tree = nullptr; 
		this->DeleteAllItems();
	}
}