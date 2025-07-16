#ifndef _CUSTOM_RAY_CASTER_HPP_
#define _CUSTOM_RAY_CASTER_HPP_
#include "CustomIntersector.hpp"
#include "threepp/math/Vector3.hpp"
#include "threepp/math/Vector2.hpp"
#include <memory>
#include <vector>
namespace threepp
{
	class BufferGeometry;
	class Camera;
}
namespace dragon
{
	template<typename T>
	struct Ray {
		T org[3];        // [in] must set
		T dir[3];        // [in] must set
		T min_t;         // [in] must set
		T max_t;         // [in] must set
		unsigned int type;  // optional. ray type.
	};

	class CustomRayCaster
	{
	public:
		struct Result
		{
			float t{ 0.0f }; // hit distance
			float u{ 0.0f }; // barycentric coordinate u
			float v{ 0.0f }; // barycentric coordinate v
			threepp::Vector3 P{}; 
			unsigned int prim_id{ 0 }; // primitive ID
		};
	public:
		CustomRayCaster();
		~CustomRayCaster();
	public:
		bool buildBVH(std::vector<float>& vertices,
			std::vector<unsigned int>& indices);
		bool intersectObjects(Result& result);
		void setFromCamera(const threepp::Vector2& coords, threepp::Camera& camera);
		void clearBVH();
		nanort::CustomIntersector<>* getIntersector();
	private:
		std::unique_ptr<nanort::BVHAccel<float>> m_BVHAccel{ nullptr };
		std::unique_ptr<nanort::CustomIntersector<>> triangle_intersecter{ nullptr };
		nanort::Ray<float> m_Ray;
		threepp::Vector3 m_RayOrigin;
		threepp::Vector3 m_RayDirection;
	};
}
#endif // !_CUSTOM_RAY_CASTER_HPP_