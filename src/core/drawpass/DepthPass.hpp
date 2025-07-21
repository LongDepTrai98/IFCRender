#pragma once 
#ifndef _DEPTH_PASS_HPP_
#define _DEPTH_PASS_HPP_
#include "DrawPass.hpp"
namespace dragon
{
	class DepthPass : public DrawPass
	{
	public:
		DepthPass(const int& width, const int& height);
	};
}
#endif // !_DEPTH_PASS_HPP_
