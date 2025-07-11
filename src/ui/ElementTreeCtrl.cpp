#include "ElementTreeCtrl.hpp"
#include "core/node/ElementTree.hpp"
#include "core/Paths.hpp"
#include "core/utils/AppHelper.hpp"
#include "spdlog/spdlog.h"
namespace dragon
{
	ElementTreeCtrl::ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style)
		: wxTreeCtrl(parent,
			wxID_ANY,
			postion,
			size,
			style)
	{
		CreateStateImages();
		bindFunc(); 
	}
	void ElementTreeCtrl::bindFunc()
	{
		this->Bind(wxEVT_TREE_STATE_IMAGE_CLICK,&ElementTreeCtrl::OnItemStateClick,this);
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
			wxTreeItemId id_item = AppendItem(idParent, str, -1, -1, new ItemData(&childNode->getData()));
			SetItemState(id_item, 1);
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
		const std::string& checkedPath = assets::Icons + "checked.png";
		const std::string& uncheckedPath = assets::Icons + "unchecked.png";
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
	void ElementTreeCtrl::OnItemStateClick(wxTreeEvent& event)
	{
		wxTreeItemId itemId = event.GetItem();
		if (!m_bIsItemClickRecursively)
		{
			DoToggleState(itemId);
		}
		else
		{
			DoToggleState(itemId); 
			std::vector<std::pair<int, ItemData*>> itemsData{};
			ItemData* ptr_data = static_cast<ItemData*>(this->GetItemData(itemId));
			itemsData.push_back({ this->GetItemState(itemId),ptr_data });
			if (this->ItemHasChildren(itemId))
			{
				wxTreeItemIdValue cookie;
				wxTreeItemId firstChild = this->GetFirstChild(itemId, cookie);
				while (firstChild.IsOk())
				{
					RecursiveChildItems(firstChild, this->GetItemState(itemId), itemsData);
					firstChild = this->GetNextChild(firstChild, cookie);
				}
			}
			if (m_ToggleStateCallBackRecursively)
				(*m_ToggleStateCallBackRecursively)(itemsData); 
		}
	}
	void ElementTreeCtrl::RecursiveChildItems(const wxTreeItemId& itemID, 
		const int& parent_item_state, 
		std::vector<std::pair<int, ItemData*>>& itemsData)
	{
		if (this->GetItemState(itemID) != parent_item_state)
		{
			DoSetItemState(itemID, parent_item_state);
		}

		ItemData* ptr_data = static_cast<ItemData*>(this->GetItemData(itemID));
		itemsData.push_back({ this->GetItemState(itemID),ptr_data });

		if (this->ItemHasChildren(itemID))
		{
			//toggle state all child items
			wxTreeItemIdValue cookie;
			wxTreeItemId firstChild = this->GetFirstChild(itemID, cookie);
			while (firstChild.IsOk())
			{
				RecursiveChildItems(firstChild, parent_item_state, itemsData);
				firstChild = this->GetNextChild(firstChild, cookie);
			}
		}
	}
	void ElementTreeCtrl::DoToggleState(const wxTreeItemId& item)
	{
		int state = GetItemState(item);
		DoSetItemState(item, state == 0 ? 1 : 0);

	}
	void ElementTreeCtrl::DoSetItemState(const wxTreeItemId& item, const int& state)
	{
		this->SetItemState(item, state);
		ItemData* ptr_data = static_cast<ItemData*>(this->GetItemData(item)); 
		if (m_ToggleStateCallBack)
			(*m_ToggleStateCallBack)({ state, ptr_data });
	}
}