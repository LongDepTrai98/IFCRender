#ifndef _CUSTOM_INTERSECTOR_HPP_
#define _CUSTOM_INTERSECTOR_HPP_
#include "nanort.hpp"
#include <vector>
#include <functional>
#include <memory>
namespace nanort
{
	template <typename T = float, class H = TriangleIntersection<T> >
	class CustomIntersector {
		typedef struct {
			T Sx;
			T Sy;
			T Sz;
			int kx;
			int ky;
			int kz;
		} RayCoeff;

	public:
		template<class M>
		CustomIntersector(const M& m)
			: vertices_(m.GetVertices()),
			faces_(m.GetFaces()),
			vertex_stride_bytes_(m.GetVertexStrideBytes()) {
		}

		template<class M>
		CustomIntersector(const M* m)
			: vertices_(m->GetVertices()),
			faces_(m->GetFaces()),
			vertex_stride_bytes_(m->GetVertexStrideBytes()) {
		}

		CustomIntersector(const T* vertices, const unsigned int* faces,
			const size_t vertex_stride_bytes)  // e.g.
			// vertex_stride_bytes
			// = 12 = sizeof(float)
			// * 3
			: vertices_(vertices),
			faces_(faces),
			vertex_stride_bytes_(vertex_stride_bytes) {
		}
		bool Intersect(T* t_inout, const unsigned int prim_index) const
		{
			if ((prim_index < trace_options_.prim_ids_range[0]) ||
				(prim_index >= trace_options_.prim_ids_range[1])) {
				return false;
			}

			// Self-intersection test.
			if (prim_index == trace_options_.skip_prim_id) {
				return false;
			}

			const unsigned int f0 = faces_[3 * prim_index + 0];
			const unsigned int f1 = faces_[3 * prim_index + 1];
			const unsigned int f2 = faces_[3 * prim_index + 2];

			//add callback function check face
			/*HARD CODE*/
			bool bCustomCheck{ true };
			if (custom_callback_checkface)
			{
				/*CHECK FACE A,B,C FOR CUSTOM HIDE GEO*/
				bCustomCheck = (*custom_callback_checkface)(prim_index);
			}

			if (!bCustomCheck)
			{
				return false;
			}

			const real3<T> p0(get_vertex_addr(vertices_, f0 + 0, vertex_stride_bytes_));
			const real3<T> p1(get_vertex_addr(vertices_, f1 + 0, vertex_stride_bytes_));
			const real3<T> p2(get_vertex_addr(vertices_, f2 + 0, vertex_stride_bytes_));

			const real3<T> A = p0 - ray_org_;
			const real3<T> B = p1 - ray_org_;
			const real3<T> C = p2 - ray_org_;

			const T Ax = A[ray_coeff_.kx] - ray_coeff_.Sx * A[ray_coeff_.kz];
			const T Ay = A[ray_coeff_.ky] - ray_coeff_.Sy * A[ray_coeff_.kz];
			const T Bx = B[ray_coeff_.kx] - ray_coeff_.Sx * B[ray_coeff_.kz];
			const T By = B[ray_coeff_.ky] - ray_coeff_.Sy * B[ray_coeff_.kz];
			const T Cx = C[ray_coeff_.kx] - ray_coeff_.Sx * C[ray_coeff_.kz];
			const T Cy = C[ray_coeff_.ky] - ray_coeff_.Sy * C[ray_coeff_.kz];

			T U = Cx * By - Cy * Bx;
			T V = Ax * Cy - Ay * Cx;
			T W = Bx * Ay - By * Ax;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif

			// Fall back to test against edges using double precision.
			if (U == static_cast<T>(0.0) || V == static_cast<T>(0.0) ||
				W == static_cast<T>(0.0)) {
				double CxBy = static_cast<double>(Cx) * static_cast<double>(By);
				double CyBx = static_cast<double>(Cy) * static_cast<double>(Bx);
				U = static_cast<T>(CxBy - CyBx);

				double AxCy = static_cast<double>(Ax) * static_cast<double>(Cy);
				double AyCx = static_cast<double>(Ay) * static_cast<double>(Cx);
				V = static_cast<T>(AxCy - AyCx);

				double BxAy = static_cast<double>(Bx) * static_cast<double>(Ay);
				double ByAx = static_cast<double>(By) * static_cast<double>(Ax);
				W = static_cast<T>(BxAy - ByAx);
			}

			if (U < static_cast<T>(0.0) || V < static_cast<T>(0.0) ||
				W < static_cast<T>(0.0)) {
				if (trace_options_.cull_back_face ||
					(U > static_cast<T>(0.0) || V > static_cast<T>(0.0) ||
						W > static_cast<T>(0.0))) {
					return false;
				}
			}

			T det = U + V + W;
			if (det == static_cast<T>(0.0)) return false;

#ifdef __clang__
#pragma clang diagnostic pop
#endif

			const T Az = ray_coeff_.Sz * A[ray_coeff_.kz];
			const T Bz = ray_coeff_.Sz * B[ray_coeff_.kz];
			const T Cz = ray_coeff_.Sz * C[ray_coeff_.kz];
			const T D = U * Az + V * Bz + W * Cz;

			const T rcpDet = static_cast<T>(1.0) / det;
			T tt = D * rcpDet;

			if (tt > (*t_inout)) {
				return false;
			}

			if (tt < t_min_) {
				return false;
			}

			(*t_inout) = tt;
			// Use Möller-Trumbore style barycentric coordinates
			// U + V + W = 1.0 and interp(p) = U * p0 + V * p1 + W * p2
			// We want interp(p) = (1 - u - v) * p0 + u * v1 + v * p2;
			// => u = V, v = W.
			u_ = V * rcpDet;
			v_ = W * rcpDet;

			return true;
		}
		void Update(T t, unsigned int prim_idx) const
		{
			t_ = t;
			prim_id_ = prim_idx;
		}
		void PrepareTraversal(const Ray<T>& ray,
			const BVHTraceOptions& trace_options) const
		{
			ray_org_[0] = ray.org[0];
			ray_org_[1] = ray.org[1];
			ray_org_[2] = ray.org[2];

			// Calculate dimension where the ray direction is maximal.
			ray_coeff_.kz = 0;
			T absDir = std::fabs(ray.dir[0]);
			if (absDir < std::fabs(ray.dir[1])) {
				ray_coeff_.kz = 1;
				absDir = std::fabs(ray.dir[1]);
			}
			if (absDir < std::fabs(ray.dir[2])) {
				ray_coeff_.kz = 2;
				absDir = std::fabs(ray.dir[2]);
			}

			ray_coeff_.kx = ray_coeff_.kz + 1;
			if (ray_coeff_.kx == 3) ray_coeff_.kx = 0;
			ray_coeff_.ky = ray_coeff_.kx + 1;
			if (ray_coeff_.ky == 3) ray_coeff_.ky = 0;

			// Swap kx and ky dimension to preserve winding direction of triangles.
			if (ray.dir[ray_coeff_.kz] < static_cast<T>(0.0))
				std::swap(ray_coeff_.kx, ray_coeff_.ky);

			// Calculate shear constants.
			ray_coeff_.Sx = ray.dir[ray_coeff_.kx] / ray.dir[ray_coeff_.kz];
			ray_coeff_.Sy = ray.dir[ray_coeff_.ky] / ray.dir[ray_coeff_.kz];
			ray_coeff_.Sz = static_cast<T>(1.0) / ray.dir[ray_coeff_.kz];

			trace_options_ = trace_options;

			t_min_ = ray.min_t;

			u_ = static_cast<T>(0.0);
			v_ = static_cast<T>(0.0);
		}
		T GetT() const { return t_; };
		void PostTraversal(const Ray<T>& ray, bool hit, H* isect) const {
			if (hit && isect) {
				(*isect).t = t_;
				(*isect).u = u_;
				(*isect).v = v_;
				(*isect).prim_id = prim_id_;
			}
			(void)ray;
		}
	public:
		const T* vertices_;
		const unsigned int* faces_;
		const size_t vertex_stride_bytes_;
		mutable real3<T> ray_org_;
		mutable RayCoeff ray_coeff_;
		mutable BVHTraceOptions trace_options_;
		mutable T t_min_;
		mutable T t_;
		mutable T u_;
		mutable T v_;
		mutable unsigned int prim_id_;
		std::shared_ptr<std::function<bool(const unsigned int)>> custom_callback_checkface{ nullptr };
	};
}
#endif // !_CUSTOM_INTERSECTOR_HPP_