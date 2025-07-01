#include "ThreeHelper.hpp"
#include "threepp/threepp.hpp"
namespace dragon
{
	void ThreeHelper::createMesh(const std::vector<float>& vertices,
		const std::vector<uint32_t>& indices,
		std::shared_ptr<threepp::Mesh>& mesh,
		std::shared_ptr<threepp::Material>& material)
	{
		//updat vertices of mesh
		if (!mesh)
		{
			mesh = threepp::Mesh::create();
		}
		mesh->geometry()->setIndex(indices);
		mesh->geometry()->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		mesh->setMaterial(material);
		//mesh->geometry()->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
	}
	std::shared_ptr<threepp::BufferGeometry> ThreeHelper::createBufferGeometry(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
	{
		std::shared_ptr<threepp::BufferGeometry> geometry = threepp::BufferGeometry::create();
		geometry->setIndex(indices);
		geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		//geometry->computeVertexNormals();
		return geometry;
	}
}