#include "CustomRayCaster.hpp"
#include "threepp/threepp.hpp"
#include "spdlog/spdlog.h"
#include <iostream>
#include <vector>

namespace dragon
{
	CustomRayCaster::CustomRayCaster()
	{
		// Constructor implementation
		if (!m_BVHAccel)
		{
			m_BVHAccel = std::make_unique<nanort::BVHAccel<float>>();
			m_Ray.min_t = 0.0f;
			m_Ray.max_t = std::numeric_limits<float>::max();
			m_Ray.org[0] = m_Ray.org[1] = m_Ray.org[2] = 0.0f;
			m_Ray.dir[0] = m_Ray.dir[1] = m_Ray.dir[2] = 0.0f;
		}
	}
	CustomRayCaster::~CustomRayCaster()
	{
		// Destructor implementation
	}
	bool CustomRayCaster::buildBVH(std::vector<float>& vertices,
		std::vector<unsigned int>& indices)
	{
		if (m_BVHAccel)
		{
			m_BVHAccel.reset();
			m_BVHAccel = nullptr;
		}
		if (triangle_intersecter)
		{
			triangle_intersecter.reset();
			triangle_intersecter = nullptr;
		}
		//auto& vertices = geometry->getAttribute<float>("position")->array();
		spdlog::info(std::format("Create BVH for Geo with Num Vertices : {}, Num indices : {}", vertices.size(), indices.size()));
		//auto& indices = geometry->getIndex()->array();
		nanort::BVHBuildOptions<float> build_options;
		nanort::TriangleMesh<float> triangle_mesh(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		nanort::TriangleSAHPred<float> triangle_pred(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		m_BVHAccel = std::make_unique<nanort::BVHAccel<float>>();
		auto ret = m_BVHAccel->Build(indices.size() / 3, triangle_mesh, triangle_pred, build_options);
		triangle_intersecter = std::make_unique<nanort::CustomIntersector<>>(vertices.data(), indices.data(), /* stride */sizeof(float) * 3);
		spdlog::info("End build BVH");
		return ret;
	}
	bool CustomRayCaster::intersectObjects(Result& intersect)
	{
		if (m_BVHAccel)
		{
			nanort::TriangleIntersection<float> isect;
			nanort::BVHTraceOptions trace_options;
			bool hit = m_BVHAccel->Traverse(m_Ray, *triangle_intersecter.get(), &isect, trace_options);
			if (hit)
			{
				if (triangle_intersecter->Intersect(&isect.t, isect.prim_id))
				{
					intersect.t = isect.t; // Set the hit distance
					intersect.u = isect.u; // Set the barycentric coordinate u
					intersect.v = isect.v; // Set the barycentric coordinate v
					intersect.prim_id = isect.prim_id; // Set the primitive ID
					/*CAL HIT POINT*/
					intersect.P = m_RayOrigin + m_RayDirection * isect.t; 
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
			this->m_RayOrigin.setFromMatrixPosition(*camera.matrixWorld);
			this->m_RayDirection.set(coords.x, coords.y, 0.5f).unproject(camera).sub(this->m_RayOrigin).normalize();
			m_Ray.org[0] = this->m_RayOrigin.x;
			m_Ray.org[1] = this->m_RayOrigin.y;
			m_Ray.org[2] = this->m_RayOrigin.z;

			m_Ray.dir[0] = this->m_RayDirection.x;
			m_Ray.dir[1] = this->m_RayDirection.y;
			m_Ray.dir[2] = this->m_RayDirection.z;
		}
		else if (camera.is<threepp::OrthographicCamera>()) {
			this->m_RayOrigin.set(coords.x, coords.y, (camera.nearPlane + camera.farPlane) / (camera.nearPlane - camera.farPlane)).unproject(camera);// set origin in plane of camera
			this->m_RayDirection.set(0, 0, -1).transformDirection(*camera.matrixWorld);
			m_Ray.org[0] = this->m_RayOrigin.x;
			m_Ray.org[1] = this->m_RayOrigin.y;
			m_Ray.org[2] = this->m_RayOrigin.z;

			m_Ray.dir[0] = this->m_RayDirection.x;
			m_Ray.dir[1] = this->m_RayDirection.y;
			m_Ray.dir[2] = this->m_RayDirection.z;
		}
	}
	void CustomRayCaster::clearBVH()
	{
		if (m_BVHAccel)
		{
			m_BVHAccel.reset();
			triangle_intersecter.reset();
		}
		m_Ray.min_t = 0.0f;
		m_Ray.max_t = std::numeric_limits<float>::max();
		m_Ray.org[0] = m_Ray.org[1] = m_Ray.org[2] = 0.0f;
		m_Ray.dir[0] = m_Ray.dir[1] = m_Ray.dir[2] = 0.0f;
	}
	nanort::CustomIntersector<>* CustomRayCaster::getIntersector()
	{
		return triangle_intersecter.get();
	}
}