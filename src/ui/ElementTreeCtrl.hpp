#ifndef _ELEMENT_TREE_CTRL_HPP_
#define _ELEMENT_TREE_CTRL_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class ElementTreeCtrl : public wxTreeCtrl
	{
	public: 
		ElementTreeCtrl(wxWindow* parent, const wxPoint& postion, const wxSize& size, long style);
	private: 
	};
}
#endif // !_ELEMENT_TREE_CTRL_HPP_
