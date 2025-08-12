#include "wxInclude.hpp"
namespace dragon::panel_config
{
	static wxAuiPaneInfo scene_view_panel_info = wxAuiPaneInfo()
		.Name("view")
		.Caption("View")
		.MinSize(640, 480)
		.MaximizeButton(true)
		.MinimizeButton(true)
		.PinButton(true)
		.CloseButton(false)
		.Dockable(true)
		//.Center()
		.PaneBorder()
		.Maximize();
	static wxAuiPaneInfo map_view_panel_info = wxAuiPaneInfo()
		.Name("map")
		.Caption("Map")
		.MinSize(640, 480)
		.MaximizeButton(true)
		.MinimizeButton(true)
		.PinButton(true)
		.CloseButton(false)
		.Dockable(true)
		.Center()
		.PaneBorder()
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