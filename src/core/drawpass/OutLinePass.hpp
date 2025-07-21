#pragma once
#ifndef _MAIN_DRAW_PASS_HPP_
#define _MAIN_DRAW_PASS_HPP_
#include "DrawPass.hpp"
namespace dragon
{
	class OutLinePass : public DrawPass
	{
	public: 
		OutLinePass(const int& width, const int& height);
	};
}
#endif // !_MAIN_DRAW_PASS_HPP_
