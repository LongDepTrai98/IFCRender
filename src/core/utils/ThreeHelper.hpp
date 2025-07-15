#ifndef _THREE_HELPER_HPP_
#define _THREE_HELPER_HPP_
#include "core/io/IFCGeometryCache.hpp"
#include <vector>
#include <memory>
#include <map>
namespace threepp
{
	class Mesh;
	class Material;
	class BufferGeometry;
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
			const std::vector<float>& normals);
		static std::shared_ptr<threepp::BufferGeometry> BuildSubGeometryWithOffset(const IFCModelCache::element& element,
			const std::vector<float>& vertices,
			const std::vector<float>& normals, 
			const std::vector<unsigned int>& indices);
	private:
		ThreeHelper() = default;
	};
}
#endif // _THREE_HELPER_HPP_