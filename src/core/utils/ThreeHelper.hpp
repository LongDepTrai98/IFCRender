#ifndef _THREE_HELPER_HPP_
#define _THREE_HELPER_HPP_
#include <vector>
#include <memory>
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
		static std::shared_ptr<threepp::BufferGeometry> createBufferGeometry(const std::vector<float>& vertices,
			const std::vector<uint32_t>& indices);
	private:
		ThreeHelper() = default;
	};
}
#endif // _THREE_HELPER_HPP_