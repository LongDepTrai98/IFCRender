#include "wxInclude.hpp"
namespace dragon::panel_config
{
	static wxAuiPaneInfo scene_view_panel_info = wxAuiPaneInfo()
		.Name("view")
		.Caption("View")
		.Right()
		.PinButton()
		.MinSize(640, 480)
		.MinimizeButton(true)
		.MaximizeButton(true)
		.CloseButton(false)
		.Dockable(true)
		.Center()
		.Maximize(); 
	static wxAuiPaneInfo tree_ctrl_panel_info = wxAuiPaneInfo()
		.Name("tree")
		.Caption("Entity")
		.Left()
		.MinimizeButton(true)
		.PinButton(true)
		.MinSize(400, 300)
		.Dockable(true);
}