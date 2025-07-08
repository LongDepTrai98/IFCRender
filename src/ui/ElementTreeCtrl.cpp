#include "ElementTreeCtrl.hpp"
#include "core/node/ElementTree.hpp"
#include "core/Paths.hpp"
namespace dragon
{
	ElementTreeCtrl::ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style)
		: wxTreeCtrl(parent,
			wxID_ANY,
			postion,
			size, 
			wxTR_DEFAULT_STYLE | wxSUNKEN_BORDER)
	{
		CreateStateImages(); 
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
			SetItemState(id_item, 0);
			AddItemsRecursively(id_item, childNode); 
		}
	}
	void ElementTreeCtrl::clearData()
	{
		m_Tree = nullptr; 
		this->DeleteAllItems();
	}
	void ElementTreeCtrl::CreateStateImages()
	{
		std::vector<wxBitmapBundle> images;
		std::vector<wxIcon> icons;
		//create icons 
		const std::string& checkedPath = assets::Icons  + "checked.png"; 
		const std::string& uncheckedPath = assets::Icons  + "unchecked.png"; 
		icons.push_back(wxIcon(uncheckedPath, wxBITMAP_TYPE_PNG));
		icons.push_back(wxIcon(checkedPath, wxBITMAP_TYPE_PNG));
		const wxSize iconSize(icons[0].GetWidth(), icons[0].GetHeight());
		for (const wxIcon& icon : icons)
		{
			images.push_back(wxBitmapBundle::FromImpl(new FixedSizeImpl(iconSize, icon)));
		}
		SetStateImages(images); 
		Update(); 
	}
}