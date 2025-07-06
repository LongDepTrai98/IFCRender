#define wxUSE_TREECTRL 1
#include "ElementTreeCtrl.hpp"
namespace dragon
{
	ElementTreeCtrl::ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style)
		: wxTreeCtrl(parent,
			10000,
			postion,
			size,
			style)
	{
	}
}