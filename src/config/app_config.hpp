#include <iostream>
#include <string>
namespace dragon::app_config
{
	constexpr const char* app_name = "Dragon";
	constexpr const int num_sampler = 4;
	constexpr const bool enable_msaa = true;
}
namespace dragon::component_id
{
	constexpr int ID_MENUBAR = 1;
}
namespace dragon::index
{
	constexpr int INDEX_MAIN_VIEWPORT = 0; 
	constexpr int INDEX_GIZMO_VIEWPORT = 1; 
	constexpr int INDEX_MODEL = 0; 
	constexpr int INDEX_OUTLINE_EDGE = 1; 
	constexpr int INDEX_LAYER_OVERLAY = 2; 
}
namespace dragon::outline_edge
{
	constexpr float THRESHOLD_ANGLE = 30.0f; 
}