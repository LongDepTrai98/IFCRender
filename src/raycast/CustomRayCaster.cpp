#include "CustomRayCaster.hpp"
#include "threepp/threepp.hpp"
#include "spdlog/spdlog.h"
#include <iostream>
namespace dragon
{
	CustomRayCaster::CustomRayCaster()
	{
		// Constructor implementation
		if (!m_BVHAccel)
		{
			m_BVHAccel = std::make_unique<nanort::BVHAccel<float>>();
			ray.min_t = 0.0f;
			ray.max_t = std::numeric_limits<float>::max();
			ray.org[0] = ray.org[1] = ray.org[2] = 0.0f;
			ray.dir[0] = ray.dir[1] = ray.dir[2] = 0.0f;
		}
	}
	CustomRayCaster::~CustomRayCaster()
	{
		// Destructor implementation
	}
	bool CustomRayCaster::buildBVH(threepp::BufferGeometry* geometry)
	{
		auto& vertices = geometry->getAttribute<float>("position")->array();
		spdlog::info(std::format("Create BVH for Geo with Num Vertices : {}, Num indices : {}", vertices.size(), geometry->getIndex()->count()));
		auto& indices = geometry->getIndex()->array();
		nanort::BVHBuildOptions<float> build_options;
		nanort::TriangleMesh<float> triangle_mesh(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		nanort::TriangleSAHPred<float> triangle_pred(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		if (!m_BVHAccel)
			m_BVHAccel = std::make_unique<nanort::BVHAccel<float>>();
		auto ret = m_BVHAccel->Build(indices.size() / 3, triangle_mesh, triangle_pred, build_options);
		triangle_intersecter = std::make_unique<nanort::TriangleIntersector<>>(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		spdlog::info("End build BVH");
		return ret;
	}
	bool CustomRayCaster::intersectObjects(Result& intersect)
	{
		if (m_BVHAccel)
		{
			nanort::TriangleIntersection<float> isect;
			nanort::BVHTraceOptions trace_options;
			bool hit = m_BVHAccel->Traverse(ray, *triangle_intersecter.get(), &isect, trace_options);
			if (hit)
			{
				if (triangle_intersecter->Intersect(&isect.t, isect.prim_id))
				{
					intersect.t = isect.t; // Set the hit distance
					intersect.u = isect.u; // Set the barycentric coordinate u
					intersect.v = isect.v; // Set the barycentric coordinate v
					intersect.prim_id = isect.prim_id; // Set the primitive ID
					return true;
				}
			}
		}
		return false;
	}
	void CustomRayCaster::setFromCamera(const threepp::Vector2& coords, threepp::Camera& camera)
	{
		if (camera.is<threepp::PerspectiveCamera>())
		{
			this->rayOrigin.setFromMatrixPosition(*camera.matrixWorld);
			this->rayDirection.set(coords.x, coords.y, 0.5f).unproject(camera).sub(this->rayOrigin).normalize();
			ray.org[0] = this->rayOrigin.x;
			ray.org[1] = this->rayOrigin.y;
			ray.org[2] = this->rayOrigin.z;

			ray.dir[0] = this->rayDirection.x;
			ray.dir[1] = this->rayDirection.y;
			ray.dir[2] = this->rayDirection.z;
		}
		else if (camera.is<threepp::OrthographicCamera>()) {
			this->rayOrigin.set(coords.x, coords.y, (camera.nearPlane + camera.farPlane) / (camera.nearPlane - camera.farPlane)).unproject(camera);// set origin in plane of camera
			this->rayDirection.set(0, 0, -1).transformDirection(*camera.matrixWorld);
			ray.org[0] = this->rayOrigin.x;
			ray.org[1] = this->rayOrigin.y;
			ray.org[2] = this->rayOrigin.z;

			ray.dir[0] = this->rayDirection.x;
			ray.dir[1] = this->rayDirection.y;
			ray.dir[2] = this->rayDirection.z;
		}
	}
	void CustomRayCaster::clearBVH()
	{
		if (m_BVHAccel)
		{
			m_BVHAccel.reset();
			triangle_intersecter.reset();
		}
		ray.min_t = 0.0f;
		ray.max_t = std::numeric_limits<float>::max();
		ray.org[0] = ray.org[1] = ray.org[2] = 0.0f;
		ray.dir[0] = ray.dir[1] = ray.dir[2] = 0.0f;
	}
}