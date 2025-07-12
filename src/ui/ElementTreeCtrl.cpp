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
		this->Bind(wxEVT_TREE_STATE_IMAGE_CLICK, &ElementTreeCtrl::OnItemStateClick, this);
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
		//this->AssignImageList(new wxImageList(1, 1));
	}
	void ElementTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent, const std::shared_ptr<TreeNode>& node)
	{
		wxString str;
		for (auto& childNode : node->children)
		{
			str.Printf(childNode->getLabelNode().c_str());
			wxTreeItemId id_item = AppendItem(idParent, str, -1, -1/*new ItemData(&childNode->getData())*/);
			ItemData::ItemValue item_value{ nullptr };
			const int& idNode = childNode->getID();
			if (m_GetData_Item_Callback)
				item_value = (*m_GetData_Item_Callback)(idNode);
			SetItemData(id_item, new ItemData(item_value));
			SetItemState(id_item, 1);
			AddItemsRecursively(id_item, childNode);
		}
	}
	void ElementTreeCtrl::createItemNode(const wxTreeItemId& itemId, std::vector<std::pair<int, ItemData*>>& itemsData)
	{
		ItemData* ptr_data = static_cast<ItemData*>(GetItemData(itemId));
		itemsData.push_back({ GetItemState(itemId),ptr_data });
		if (ItemHasChildren(itemId))
		{
			wxTreeItemIdValue cookie;
			wxTreeItemId firstChild = GetFirstChild(itemId, cookie);
			while (firstChild.IsOk())
			{
				RecursiveChildItems(firstChild, GetItemState(itemId), itemsData);
				firstChild = GetNextChild(firstChild, cookie);
			}
		}
	}
	void ElementTreeCtrl::clearData()
	{
		m_Tree = nullptr;
		DeleteAllItems();
	}
	void ElementTreeCtrl::CreateStateImages()
	{
		std::vector<wxBitmapBundle> images;
		std::vector<wxIcon> icons;
		//create icons
		const std::string& checkedPath = assets::Icons + "checked.png";
		spdlog::info("Load Resource : {}", checkedPath);
		const std::string& uncheckedPath = assets::Icons + "unchecked.png";
		spdlog::info("Load Resource : {}", uncheckedPath);
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
			createItemNode(itemId, itemsData);
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
		createItemNode(itemID, itemsData);
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