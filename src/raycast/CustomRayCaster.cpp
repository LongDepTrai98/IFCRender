#include "CustomRayCaster.hpp"
#include "threepp/threepp.hpp"
namespace dragon
{
	CustomRayCaster::CustomRayCaster()
	{
		// Constructor implementation
		if (!m_BVHAccel)
		{
			m_BVHAccel = std::make_unique<nanort::BVHAccel<float>>(); 
		}
	}
	CustomRayCaster::~CustomRayCaster()
	{
		// Destructor implementation
	}
	bool CustomRayCaster::buildBVH(threepp::BufferGeometry* geometry)
	{
		auto& vertices = geometry->getAttribute<float>("position")->array(); 
		auto& indices = geometry->getIndex()->array();
		nanort::BVHBuildOptions<float> build_options;
		nanort::TriangleMesh<float> triangle_mesh(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		nanort::TriangleSAHPred<float> triangle_pred(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		auto ret = m_BVHAccel->Build(indices.size() / 3, triangle_mesh, triangle_pred, build_options);
		return ret;
	}
}