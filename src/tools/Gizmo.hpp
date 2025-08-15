#pragma once 
#ifndef _GIZMO_HPP_
#define _GIZMO_HPP_
#include <memory>
namespace threepp
{
	class Group; 
	class Object3D; 
	class Mesh; 
}
namespace dragon
{
	class Gizmo
	{
	public:
		std::shared_ptr<threepp::Group> create(threepp::Object3D* target_);
		std::shared_ptr<threepp::Mesh> createXYPlaneHelper(float size); 
		std::shared_ptr<threepp::Mesh> createXZPlaneHelper(float size); 
		std::shared_ptr<threepp::Mesh> createYZPlaneHelper(float size); 
		//createXYPlaneHelper
	public: 
		float arrow_length{ 300.0f }; 
		float arrow_head_length{ 100.0f };
		float arrow_head_width{ 100.0f };
		float plane_size{ 150.0f }; 
		float padding_plane{ 50.0f }; 
		threepp::Object3D* target{ nullptr }; 
		std::shared_ptr<threepp::Group> gizmo{ nullptr }; 
	};
}
#endif // !_GIZMO_HPP_
