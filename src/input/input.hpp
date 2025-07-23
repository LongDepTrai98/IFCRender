#pragma
#ifndef _INPUT_HPP_
#define _INPUT_HPP_
#include "threepp/math/Vector2.hpp"
#include "threepp/math/infinity.hpp"
#include "wx/event.h"
namespace threepp
{
	class OrbitControls;
	class Camera;
}
namespace dragon
{
	struct MouseState {
		bool isLButtonDown{ false };
		bool isRButtonDown{ false };
		threepp::Vector2 nor_mouse_pos{ -threepp::Infinity<float>, -threepp::Infinity<float> };
	};

	struct EventData
	{
		threepp::OrbitControls* control{ nullptr };
		threepp::Vector2 p{};
		threepp::Camera* camera{ nullptr };
	};

	struct KeyData
	{
		wxKeyEvent key;
	};

	struct ToolBarData
	{
		wxCommandEvent event;
		bool bIsCheck;
	};
}
#endif // !_INPUT_HPP_