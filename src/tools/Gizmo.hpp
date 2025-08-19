#pragma once 
#ifndef _GIZMO_HPP_
#define _GIZMO_HPP_
#include <memory>
#include "threepp/math/Plane.hpp"
#include "threepp/math/Vector3.hpp"
#include "threepp/math/Ray.hpp"
#include "threepp/math/Quaternion.hpp"
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
		enum class MODE
		{
			TRANSLATE,
			ROTATE
		};
		struct RotState {
			bool dragging{ false };
			std::string axisName;
			threepp::Quaternion startObjQuat{};
			threepp::Vector3    ringNormalWorld{};
			threepp::Vector3 vPrev;   
			float totalAngle{ 0.0f };
		}; 
	public:
		std::shared_ptr<threepp::Group> create();
		std::shared_ptr<threepp::Mesh> createXYPlaneHelper(float size); 
		std::shared_ptr<threepp::Mesh> createXZPlaneHelper(float size); 
		std::shared_ptr<threepp::Mesh> createYZPlaneHelper(float size);
	public: 
		void setTarget(threepp::Object3D* target_); 
		void startDrag(threepp::Ray& ray, threepp::Vector3& camDirection, threepp::Vector3& selected_axis, bool isAxis_); 
		void updateDrag(threepp::Ray& ray, threepp::Vector3 camDir);
		void switchMode(MODE mode);
		void endDrag();
	public: 
		float arrow_length{ 50.0f }; 
		float plane_size{ 25.0f }; 
		float padding_plane{ 10.0f }; 
		threepp::Object3D* target{ nullptr }; 
		std::shared_ptr<threepp::Group> gizmo{ nullptr };
		threepp::Plane dragPlane;
		threepp::Vector3 startPoint{0.0,0.0,0.0}; 
		threepp::Vector3 selected_axis{0.0,0.0,0.0}; 
		bool isAxis{ false }; 
		MODE currentMode{ MODE::TRANSLATE };
		RotState rs; 
	};
}
#endif // !_GIZMO_HPP_
