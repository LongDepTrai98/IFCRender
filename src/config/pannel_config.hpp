#include "wxInclude.hpp"
namespace dragon::panel_config
{
	static wxAuiPaneInfo scene_view_panel_info = wxAuiPaneInfo().Name("view").Caption("View").MinSize(500, 300).MaximizeButton(true).Right().Dockable(true);
	static wxAuiPaneInfo tree_ctrl_panel_info = wxAuiPaneInfo().Name("tree").Caption("Entity").Left().CloseButton(true).MaximizeButton(false).PinButton(true).MinSize(300, 300).Dockable(true);
}