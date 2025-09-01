#pragma once
#ifndef _CESIUM_HELPER_HPP_
#define _CESIUM_HELPER_HPP_
#include <memory>
#include <vector>
namespace threepp
{
	class Group;
}
namespace dragon
{
	class CesiumHelper
	{
	public: 
		static std::shared_ptr<threepp::Group> createGLB(std::vector<std::byte>& bytes);
	};
}
#endif // !_CESIUM_HELPER_HPP_