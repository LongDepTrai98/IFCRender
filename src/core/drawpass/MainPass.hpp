#pragma once
#ifndef _MAIN_PASS_HPP_
#define _MAIN_PASS_HPP_
#include "DrawPass.hpp"
namespace dragon
{
	class MainPass : public DrawPass
	{
	public:
		MainPass(const int& width, const int& height);
	public:
		void render(threepp::GLRenderer* renderer, threepp::Camera* camera) override;
		void clear() override;
	};
}
#endif // !_MAIN_PASS_HPP_