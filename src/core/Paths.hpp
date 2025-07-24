#pragma once
#ifndef _PATHS_HPP_
#define _PATHS_HPP_
#include <string>
namespace dragon::assets
{
	inline const std::string Root = "assets/";
	inline const std::string Icons = Root + "icons/";
	inline const std::string Models = Root + "models/";
	inline const std::string Cube = Models + "cube.stl";
	inline const std::string Toolbar = Icons + "toolbar/";
	inline const std::string ButtonToolBar_OpenFolder = Toolbar + "open-folder.png";
	inline const std::string ButtonToolBar_SetPivot = Toolbar + "pivot.png";
	inline const std::string ButtonToolBar_Hover = Toolbar + "hover.png";
	inline const std::string ButtonToolBar_DrawEdge = Toolbar + "edge.png";
	inline const std::string ButtonToolBar_DrawGrid = Toolbar + "grid.png";
	inline const std::string ButtonToolBar_MultiSelect = Toolbar + "multi_select.png";
	inline const std::string ButtonToolBar_Debug = Toolbar + "debug.png";
	inline const std::string Shader = "shaders/";
}
#endif // !_PATHS_HPP_