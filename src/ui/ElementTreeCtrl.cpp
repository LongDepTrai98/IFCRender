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
		CreateRoot(); 
		bindFunc();
	}
	void ElementTreeCtrl::bindFunc()
	{
		this->Bind(wxEVT_TREE_STATE_IMAGE_CLICK, &ElementTreeCtrl::OnItemStateClick, this);
	}
	void ElementTreeCtrl::setData(std::shared_ptr<ElementTree> treeData, const std::string& key_root)
	{
		wxTreeItemId root_item_add; 
		std::string name_root_item_add; 
		/*HARD CODE*/
		if (key_root == "bim")
		{
			root_item_add = root_bim_item_id;
			name_root_item_add = "bim"; 
		}
		else if (key_root == "map")
		{
			root_item_add = root_map_item_id;
			name_root_item_add = "map"; 
		}
		this->DeleteChildren(root_item_add);
		//m_Tree = treeData;
		//auto parent = m_Tree->m_Parent;
		auto parent = treeData->m_Parent;
		wxString str;
		str.Printf(parent->getLabelNode().c_str());
		wxTreeItemId id_item = AppendItem(root_item_add, str, -1, -1);
		ItemData::ItemValue item_value{ nullptr };
		const int& idNode = parent->getID();
		auto GetData_Item_Callback = m_umap_callback[key_root].m_GetData_Item_Callback.get(); 
		if (GetData_Item_Callback)
			item_value = (*GetData_Item_Callback)(idNode,str.ToStdString());
		SetItemData(id_item, new ItemData(item_value,name_root_item_add));
		SetItemState(id_item, 1);
		if (parent->children.size() != 0)
		{
			auto t = ICON::EXPAND; 
			SetItemImage(id_item, (int)ICON::EXPAND, wxTreeItemIcon_Normal);
		}
		else
		{
			SetItemImage(id_item, (int)m_umap_callback[name_root_item_add].icon_item_type, wxTreeItemIcon_Normal);
		}
		AddItemsRecursively(id_item,
			parent,
			name_root_item_add);
		this->ExpandAll();
	}
	void ElementTreeCtrl::AddItemsRecursively(const wxTreeItemId& idParent,
		const std::shared_ptr<TreeNode>& node, 
		const std::string& name)
	{
		wxString str;
		for (auto& childNode : node->children)
		{
			str.Printf(childNode->getLabelNode().c_str());
			wxTreeItemId id_item = AppendItem(idParent, str, -1, -1);
			ItemData::ItemValue item_value{ nullptr };
			const int& idNode = childNode->getID();
			auto GetData_Item_Callback = m_umap_callback[name].m_GetData_Item_Callback.get(); 
			if (GetData_Item_Callback)
				item_value = (*GetData_Item_Callback)(idNode,str.ToStdString());
			SetItemData(id_item, new ItemData(item_value,name));
			SetItemState(id_item, 1);
			if (childNode->children.size() != 0)
			{
				SetItemImage(id_item, (int)ICON::EXPAND, wxTreeItemIcon_Normal);
			}
			else
			{
				SetItemImage(id_item, (int)m_umap_callback[name].icon_item_type, wxTreeItemIcon_Normal);
			}
			AddItemsRecursively(id_item,
				childNode,
				name);
		}
	}
	void ElementTreeCtrl::createItemNode(const wxTreeItemId& itemId, 
		std::vector<std::pair<int, ItemData*>>& itemsData)
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
		const std::string& item_img_path = assets::Icons + "item_tree.ico";
		const std::string& item_expand_img_path = assets::Icons + "item_expand_img.ico";
		const std::string& item_scene_img_path = assets::Icons + "scene_tree.ico";
		const std::string& item_map_img_path = assets::Icons + "map.ico";
		const std::string& item_bim_img_path = assets::Icons + "bim.ico";
		const std::string& item_layer_img_path = assets::Icons + "layer.ico";
		images.push_back(AppHelper::loadBitmapBundle(item_img_path, wxBITMAP_TYPE_ICO));
		images.push_back(AppHelper::loadBitmapBundle(item_expand_img_path, wxBITMAP_TYPE_ICO));
		images.push_back(AppHelper::loadBitmapBundle(item_scene_img_path, wxBITMAP_TYPE_ICO));
		images.push_back(AppHelper::loadBitmapBundle(item_map_img_path, wxBITMAP_TYPE_ICO));
		images.push_back(AppHelper::loadBitmapBundle(item_bim_img_path, wxBITMAP_TYPE_ICO));
		images.push_back(AppHelper::loadBitmapBundle(item_layer_img_path, wxBITMAP_TYPE_ICO));
		SetImages(images);
	}
	void ElementTreeCtrl::CreateStateImages()
	{
		std::vector<wxBitmapBundle> images;
		std::vector<wxIcon> icons;
		//create icons
		const std::string& checkedPath = assets::Icons + "checked.ico";
		const std::string& uncheckedPath = assets::Icons + "unchecked.ico";
		images.push_back(AppHelper::loadBitmapBundle(uncheckedPath, wxBITMAP_TYPE_ICO));
		images.push_back(AppHelper::loadBitmapBundle(checkedPath, wxBITMAP_TYPE_ICO));
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
			ItemData* ptr_data = static_cast<ItemData*>(this->GetItemData(itemId));
			std::string name = ptr_data->GetName(); 
			DoToggleState(itemId);
			std::vector<std::pair<int, ItemData*>> itemsData{};
			createItemNode(itemId, itemsData);
			auto ToggleStateCallBackRecursively = m_umap_callback[name].m_ToggleStateCallBackRecursively.get(); 
			if (ToggleStateCallBackRecursively)
				(*ToggleStateCallBackRecursively)(itemsData);
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
		std::string name = ptr_data->GetName(); 
		auto toggleStateCallback = m_umap_callback["name"].m_ToggleStateCallBack.get(); 
		if (toggleStateCallback)
			(*toggleStateCallback)({ state, ptr_data });
	}
	void ElementTreeCtrl::CreateRoot()
	{
		clearData();
		wxString str("Scene");
		root_item_id = this->AddRoot(str);
		ItemData::ItemValue item_value{ nullptr };
		SetItemImage(root_item_id, (int)ICON::SCENE, wxTreeItemIcon_Normal);
		str.Printf("Map");
		root_map_item_id = AppendItem(root_item_id, str, -1, -1);
		SetItemState(root_map_item_id, 1);
		SetItemImage(root_map_item_id, (int)ICON::MAP, wxTreeItemIcon_Normal);
		SetItemData(root_map_item_id, new ItemData(item_value,"map"));
		func_callback map_callback; 
		m_umap_callback.insert({ "map",func_callback()});
		m_umap_callback["map"].icon_item_type = ICON::ITEM_LAYER; 
		str.Printf("Bim"); 
		root_bim_item_id = AppendItem(root_item_id,str,-1,-1);
		SetItemImage(root_bim_item_id, (int)ICON::BIM, wxTreeItemIcon_Normal);
		SetItemState(root_bim_item_id, 1); 
		SetItemData(root_bim_item_id, new ItemData(item_value,"bim"));
		m_umap_callback.insert({"bim",func_callback()});
		m_umap_callback["bim"].icon_item_type = ICON::ITEM_BIM; 
		this->ExpandAll(); 
	}
}