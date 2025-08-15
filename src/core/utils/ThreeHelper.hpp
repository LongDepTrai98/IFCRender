#ifndef _THREE_HELPER_HPP_
#define _THREE_HELPER_HPP_
#include "core/io/IFCGeometryCache.hpp"
#include "threepp/math/Vector3.hpp"
#include "threepp/math/Matrix4.hpp"
#include <vector>
#include <memory>
#include <map>
namespace threepp
{
	class Mesh;
	class Material;
	class BufferGeometry;
	class BufferAttribute;
}
namespace dragon
{
	class ThreeHelper
	{
	public:
		static void createMesh(const std::vector<float>& vertices,
			const std::vector<uint32_t>& indices,
			std::shared_ptr<threepp::Mesh>& mesh,
			std::shared_ptr<threepp::Material>& material);
		static std::shared_ptr<threepp::BufferGeometry> CreateBufferGeometry(const std::vector<float>& vertices,
			const std::vector<uint32_t>& indices);
		static std::shared_ptr<threepp::BufferGeometry> BuildSubGeometry(const int& total_indices,
			const std::vector<std::shared_ptr<threepp::Material>>& materials,
			const std::map<int, std::vector<std::pair<int, int>>>& viewGeometries,
			const std::vector<unsigned int>& root_indices,
			const std::vector<float>& vertices,
			const std::vector<float>& normals,
			const threepp::Matrix4& root_matrix,
			const std::vector<unsigned int>& root_expressID = {});
		static std::shared_ptr<threepp::BufferGeometry> BuildSubGeometryWithOffset2(const std::map<int, std::vector<IFCModelCache::offset>>& view_geometries_offset_with_material,
			const std::vector<float>& vertices,
			const std::vector<float>& normals,
			const std::vector<unsigned int>& indices,
			const std::vector<std::shared_ptr<threepp::Material>>& materials,
			std::vector<std::shared_ptr<threepp::Material>>& new_materials);
		static std::shared_ptr<threepp::BufferGeometry> BuildSubGeometryWithOffset(IFCModelCache::element& element,
			const std::vector<float>& vertices,
			const std::vector<float>& normals,
			const std::vector<unsigned int>& indices);
		static void scaleAroundPivot(std::shared_ptr<threepp::BufferGeometry> geometry,
			const threepp::Vector3& pivot,
			const float& scaleX,
			const float& scaleY,
			const float& scaleZ
		);
		static threepp::Matrix4 createMatrixScaleAroundPivot(
			const threepp::Vector3& pivot,
			const float& scaleX,
			const float& scaleY,
			const float& scaleZ
		);
		static threepp::Matrix4 createMatrixRotateAroundPivot(const threepp::Vector3& pivot,
			const float& rotateX,
			const float& rotateY,
			const float& rotateZ);
		static threepp::Matrix4 createMatrixTranslateAroundPivot(const threepp::Vector3& pivot, 
			const float& tar_x,
			const float& tar_y,
			const float& tar_z);
	private:
		ThreeHelper() = default;
	};
}
#endif // _THREE_HELPER_HPP_