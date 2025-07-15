#pragma
#ifndef _INPUT_HPP_
#define _INPUT_HPP_
#include "threepp/math/Vector2.hpp"
#include "threepp/math/infinity.hpp"
namespace dragon
{
	struct MouseState {
		bool isLButtonDown{ false };
		bool isRButtonDown{ false };
		threepp::Vector2 nor_mouse_pos{ -threepp::Infinity<float>, -threepp::Infinity<float> };
	};
}
#endif // !_INPUT_HPP_
