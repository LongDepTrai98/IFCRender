#ifndef _CUSTOM_RAY_CASTER_HPP_
#define _CUSTOM_RAY_CASTER_HPP_
#include "nanort.hpp"
#include <memory>
namespace threepp
{
	class BufferGeometry;
}
namespace dragon
{
	template<typename T>
	struct Ray{
		T org[3];        // [in] must set
		T dir[3];        // [in] must set
		T min_t;         // [in] must set
		T max_t;         // [in] must set
		unsigned int type;  // optional. ray type.
	};

	struct BVHTraceOptions {
		// Trace rays only in face ids range. faceIdsRange[0] < faceIdsRange[1]
		// default: 0 to 0x3FFFFFFF(2G faces)
		unsigned int prim_ids_range[2];
		bool cull_back_face; // default: false
	};

	class CustomRayCaster
	{
	public: 
		struct RaycastParams 
		{
			float minDistance{ 0.0f };
			float maxDistance{ std::numeric_limits<float>::max() };
		};
	public:
		CustomRayCaster();
		~CustomRayCaster();
	public: 
		bool buildBVH(threepp::BufferGeometry* geometry);
	private:
		std::unique_ptr<nanort::BVHAccel<float>> m_BVHAccel{ nullptr };
		nanort::Ray<float> ray; 
	};
}
#endif // !_CUSTOM_RAY_CASTER_HPP_
