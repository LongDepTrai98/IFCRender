#include "ThreeHelper.hpp"
#include "threepp/threepp.hpp"
#include "spdlog/spdlog.h"
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
	std::shared_ptr<threepp::BufferGeometry> ThreeHelper::CreateBufferGeometry(const std::vector<float>& vertices, const std::vector<uint32_t>& indices)
	{
		std::shared_ptr<threepp::BufferGeometry> geometry = threepp::BufferGeometry::create();
		geometry->setIndex(indices);
		geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		geometry->computeVertexNormals();
		return geometry;
	}
	std::shared_ptr<threepp::BufferGeometry> ThreeHelper::BuildSubGeometry(const int& total_indices,
		const std::vector<std::shared_ptr<threepp::Material>>& materials,
		const std::map<int, std::vector<std::pair<int, int>>>& viewGeometries, 
		const std::vector<unsigned int>& root_indices, 
		const std::vector<float>& vertices, 
		const std::vector<float>& normals
		)
	{
		int index_offset{ 0 };
		std::vector<unsigned int> rebuild_indices{};
		rebuild_indices.reserve(total_indices);
		std::vector<threepp::GeometryGroup> groups;
		groups.reserve(viewGeometries.size());
		std::vector<std::shared_ptr<threepp::Material>> mats;
		int start = 0;

		for (const auto& [material_index, offsets] : viewGeometries)
		{
			int count{ 0 }; 
			threepp::GeometryGroup group;
			group.start = start;
			group.materialIndex = material_index;
			if (offsets.size() == 0)
				continue; 
			for (auto& [begin, end] : offsets)
			{
				count += (end - begin) + 1;
				rebuild_indices.insert(
					rebuild_indices.end(),
					root_indices.begin() + begin,
					root_indices.begin() + end + 1
				);
			}
			group.count = count;
			start += count;
			groups.emplace_back(group);
			mats.emplace_back(materials[material_index]);
		}

		/*for (int i = 0; i < materials.size(); ++i)
		{
			int count{ 0 };
			threepp::GeometryGroup group;
			group.start = start;
			group.materialIndex = i;
			auto& set = viewGeometries[i];
			if (set.size() == 0)
				continue;
			for (auto& [begin, end] : set)
			{
				count += (end - begin) + 1;
				rebuild_indices.insert(
					rebuild_indices.end(),
					root_indices.begin() + begin,
					root_indices.begin() + end + 1
				);
			}
			group.count = count;
			start += count;
			groups.emplace_back(group);
			mats.emplace_back(materials[i]);
		}*/
		spdlog::info("indices {}", rebuild_indices.size());
		std::shared_ptr<threepp::BufferGeometry> sub_geometry_buffer = threepp::BufferGeometry::create();
		sub_geometry_buffer->setIndex(rebuild_indices);
		sub_geometry_buffer->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		sub_geometry_buffer->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
		sub_geometry_buffer->groups = groups;
		sub_geometry_buffer->computeVertexNormals();
		return sub_geometry_buffer;
	}
	std::shared_ptr<threepp::BufferGeometry> ThreeHelper::BuildSubGeometryWithOffset(const IFCModelCache::element& element, const std::vector<float>& vertices, const std::vector<float>& normals)
	{
		std::shared_ptr<threepp::BufferGeometry> sub_geometry_buffer{ nullptr };
		return sub_geometry_buffer; 
	}
}