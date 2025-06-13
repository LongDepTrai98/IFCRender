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
		.PaneBorder(false)
		.CloseButton(false)
		.Dockable(true);
	static wxAuiPaneInfo tree_ctrl_panel_info = wxAuiPaneInfo()
		.Name("tree")
		.Caption("Entity")
		.Left()
		.MinimizeButton(true)
		.MaximizeButton(false)
		.PinButton(true)
		.MinSize(200, 300)
		.Dockable(true);
}