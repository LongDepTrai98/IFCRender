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
		CreateItemImages(); 
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
		wxString str("Scene"); 
		wxTreeItemId root_id = this->AddRoot(str);
		SetItemImage(root_id, ElementTreeCtrl::SCENE, wxTreeItemIcon_Normal);
		str.Printf(parent->getLabelNode().c_str());
		wxTreeItemId id_item = AppendItem(root_id, str, -1, -1);
		ItemData::ItemValue item_value{ nullptr };
		const int& idNode = parent->getID();
		if (m_GetData_Item_Callback)
			item_value = (*m_GetData_Item_Callback)(idNode);
		SetItemData(id_item, new ItemData(item_value));
		SetItemState(id_item, 1);
		if (parent->children.size() != 0)
		{
			SetItemImage(id_item, ElementTreeCtrl::EXPAND, wxTreeItemIcon_Normal);
		}
		else
		{
			SetItemImage(id_item, ElementTreeCtrl::ITEM, wxTreeItemIcon_Normal);
		}
		AddItemsRecursively(id_item, parent);
		this->ExpandAll();
	}
	void ElementTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent, const std::shared_ptr<TreeNode>& node)
	{
		wxString str;
		for (auto& childNode : node->children)
		{
			str.Printf(childNode->getLabelNode().c_str());
			wxTreeItemId id_item = AppendItem(idParent, str, -1, -1);
			ItemData::ItemValue item_value{ nullptr };
			const int& idNode = childNode->getID();
			if (m_GetData_Item_Callback)
				item_value = (*m_GetData_Item_Callback)(idNode);
			SetItemData(id_item, new ItemData(item_value));
			SetItemState(id_item, 1);
			if (childNode->children.size() != 0)
			{
				SetItemImage(id_item, ElementTreeCtrl::EXPAND, wxTreeItemIcon_Expanded);
			}
			else
			{
				SetItemImage(id_item, ElementTreeCtrl::ITEM, wxTreeItemIcon_Normal);
			}
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
	void ElementTreeCtrl::CreateItemImages()
	{
		wxVector<wxBitmapBundle> images;
		const std::string& item_img_path = assets::Icons + "item_tree.png"; 
		const std::string& item_expand_img_path = assets::Icons + "item_expand_img.png"; 
		const std::string& item_scene_img_path = assets::Icons + "scene_tree.png"; 
		images.push_back(AppHelper::loadBitmapBundle(item_img_path, wxBITMAP_TYPE_PNG));
		images.push_back(AppHelper::loadBitmapBundle(item_expand_img_path, wxBITMAP_TYPE_PNG));
		images.push_back(AppHelper::loadBitmapBundle(item_scene_img_path, wxBITMAP_TYPE_PNG));
		SetImages(images); 
	}
	void ElementTreeCtrl::CreateStateImages()
	{
		std::vector<wxBitmapBundle> images;
		std::vector<wxIcon> icons;
		//create icons
		const std::string& checkedPath = assets::Icons + "checked.png";
		const std::string& uncheckedPath = assets::Icons + "unchecked.png";
		images.push_back(AppHelper::loadBitmapBundle(uncheckedPath, wxBITMAP_TYPE_PNG));
		images.push_back(AppHelper::loadBitmapBundle(checkedPath, wxBITMAP_TYPE_PNG));
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