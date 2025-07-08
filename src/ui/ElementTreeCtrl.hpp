#ifndef _ELEMENT_TREE_CTRL_HPP_
#define _ELEMENT_TREE_CTRL_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class ElementTree;
	class TreeNode;

    class FixedSizeImpl : public wxBitmapBundleImpl
    {
    public:
        FixedSizeImpl(const wxSize& sizeDef, const wxIcon& icon)
            : m_sizeDef(sizeDef),
            m_icon(icon)
        {
        }

        wxSize GetDefaultSize() const override
        {
            return m_sizeDef;
        }

        wxSize GetPreferredBitmapSizeAtScale(double scale) const override
        {
            return m_sizeDef * scale;
        }

        wxBitmap GetBitmap(const wxSize& size) override
        {
            wxBitmap bmp(m_icon);
            if (size != bmp.GetSize())
                wxBitmap::Rescale(bmp, size);

            return bmp;
        }

    private:
        const wxSize m_sizeDef;
        const wxIcon m_icon;
    };


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
	public: 
		void setData(std::shared_ptr<ElementTree> treeData);
		void AddItemsRecursively(const wxTreeItemId& idParent, const std::shared_ptr<TreeNode>& node);
		void clearData();
		void CreateStateImages(); 
	private: 
		std::shared_ptr<ElementTree> m_Tree{ nullptr };
	};
}
#endif // !_ELEMENT_TREE_CTRL_HPP_
