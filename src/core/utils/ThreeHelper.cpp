#include "ThreeHelper.hpp"
#include "threepp/threepp.hpp"
#include "spdlog/spdlog.h"
#include <numeric>
#include <algorithm>
#include <execution>

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
		const std::vector<float>& normals, 
		const std::vector<unsigned int>& root_expressID
	)
	{
		/*int index_offset{ 0 };
		std::vector<unsigned int> rebuild_indices{};
		rebuild_indices.resize(total_indices);
		std::vector<threepp::GeometryGroup> groups;
		groups.reserve(viewGeometries.size());
		int start = 0;
		int cursor = 0;
		for (auto [material_index, offsets] : viewGeometries)
		{
			if (offsets.size() == 0)
				continue;
			int count{ 0 };
			threepp::GeometryGroup group;
			group.start = start;
			group.materialIndex = material_index;
			std::sort(offsets.begin(), offsets.end(), [](const auto& a, const auto& b) {
				return a.first < b.first;
				});
			for (auto& [begin, end] : offsets)
			{
				const int range_size = (end - begin) + 1;
				count += range_size;
				std::memcpy(
					rebuild_indices.data() + cursor,
					root_indices.data() + begin,
					range_size * sizeof(unsigned int)
				);
				cursor += range_size;
			}
			group.count = count;
			start += count;
			groups.emplace_back(group);
		}
		spdlog::info("indices {}", rebuild_indices.size());
		std::shared_ptr<threepp::BufferGeometry> sub_geometry_buffer = threepp::BufferGeometry::create();
		sub_geometry_buffer->setIndex(std::move(rebuild_indices));
		sub_geometry_buffer->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		sub_geometry_buffer->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
		sub_geometry_buffer->groups = std::move(groups);
		sub_geometry_buffer->computeVertexNormals();
		sub_geometry_buffer->computeBoundingBox();
		sub_geometry_buffer->computeBoundingSphere();*/
		
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
			auto _offsets = offsets; 
			threepp::GeometryGroup group;
			group.start = start;
			group.materialIndex = material_index;
			if (offsets.size() == 0)
				continue;
			std::sort(_offsets.begin(), _offsets.end(), [](const auto& a, const auto& b) {
				return a.first < b.first;
				});
			for (auto& [begin, end] : _offsets)
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
		spdlog::info("indices {}", rebuild_indices.size());
		std::shared_ptr<threepp::BufferGeometry> sub_geometry_buffer = threepp::BufferGeometry::create();
		sub_geometry_buffer->setIndex(rebuild_indices);
		sub_geometry_buffer->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3));
		sub_geometry_buffer->setAttribute("normal", threepp::FloatBufferAttribute::create(normals, 3));
		sub_geometry_buffer->setAttribute("expressID", threepp::IntBufferAttribute::create(root_expressID, 1)); 
		sub_geometry_buffer->groups = groups;
		sub_geometry_buffer->computeVertexNormals();
		sub_geometry_buffer->computeBoundingBox();
		sub_geometry_buffer->computeBoundingSphere();
		return sub_geometry_buffer;
	}

	std::shared_ptr<threepp::BufferGeometry> ThreeHelper::BuildSubGeometryWithOffset(IFCModelCache::element& element,
		const std::vector<float>& vertices,
		const std::vector<float>& normals,
		const std::vector<unsigned int>& indices)
	{
		std::shared_ptr<threepp::BufferGeometry> sub_geometry_buffer = threepp::BufferGeometry::create();
		std::vector<std::vector<float>> vecLstVertices{};
		std::vector<std::vector<unsigned int>> vecLstIndices{};
		std::vector<std::vector<float>> vecLstNormals{};
		int countVertices{ 0 };
		int countIndices{ 0 };
		int countNormals{ 0 };

		std::sort(std::execution::par, element.offsets.begin(), element.offsets.end(), [](const IFCModelCache::offset& a, const IFCModelCache::offset& b) {
			return a.begin_indices_offset < b.begin_indices_offset;
			});
		for (auto& offset : element.offsets)
		{
			std::vector<unsigned int> sub_indices{};
			const int range_index = offset.end_indices_offset - offset.begin_indices_offset + 1;
			sub_indices.reserve(range_index);
			sub_indices.insert(sub_indices.end(),
				indices.begin() + offset.begin_indices_offset,
				indices.begin() + offset.end_indices_offset + 1);
			unsigned int min_index = *std::min_element(sub_indices.begin(), sub_indices.end());
			std::vector<unsigned int> result(range_index);
			std::transform(sub_indices.begin(), sub_indices.end(), result.begin(),
				[min_index](float v) { return v - min_index; });
			vecLstIndices.emplace_back(result);
			countIndices += range_index;
			sub_indices.clear();
			const int range_vertex = offset.end_vertex_offset - offset.begin_vertex_offset + 1;
			std::vector<float> sub_vertex{};
			sub_vertex.reserve(range_vertex);
			sub_vertex.insert(sub_vertex.end(),
				vertices.begin() + offset.begin_vertex_offset,
				vertices.begin() + offset.end_vertex_offset + 1);
			vecLstVertices.emplace_back(sub_vertex);
			countVertices += range_vertex;
			std::vector<float> sub_normals{};
			sub_normals.reserve(range_vertex);
			sub_normals.insert(sub_normals.end(),
				normals.begin() + offset.begin_vertex_offset,
				normals.begin() + offset.end_vertex_offset + 1);
			vecLstNormals.emplace_back(sub_normals);
			countNormals += range_vertex;
		}

		std::vector<float> mergeVetices{};
		std::vector<unsigned int> mergeIndices{};
		std::vector<float> mergeNormals{};
		mergeVetices.reserve(countVertices);
		mergeIndices.reserve(countIndices);
		mergeNormals.reserve(countNormals);

		/*MERGE VERTICES*/
		for (const auto& vecVertex : vecLstVertices)
		{
			mergeVetices.insert(mergeVetices.end(),
				vecVertex.begin(),
				vecVertex.end());
		}

		/*MERGE NORMALS*/
		for (const auto& vecNormal : vecLstNormals)
		{
			mergeNormals.insert(mergeNormals.end(),
				vecNormal.begin(),
				vecNormal.end());
		}

		/*MERGE INDICES*/
		int offset_indices{ 0 };
		size_t i = 0;
		for (const auto& vecIndices : vecLstIndices)
		{
			std::vector<unsigned int> result(vecIndices.size());
			std::transform(vecIndices.begin(), vecIndices.end(), result.begin(),
				[offset_indices](unsigned int v) { return v + offset_indices; });
			const int count = vecLstVertices[i].size() / 3;
			offset_indices += count;
			mergeIndices.insert(mergeIndices.end(),
				result.begin(),
				result.end());
			++i;
		}
		sub_geometry_buffer->setIndex(mergeIndices);
		sub_geometry_buffer->setAttribute("position", threepp::FloatBufferAttribute::create(mergeVetices, 3));
		sub_geometry_buffer->setAttribute("normal", threepp::FloatBufferAttribute::create(mergeNormals, 3));
		sub_geometry_buffer->computeBoundingBox();
		sub_geometry_buffer->computeBoundingSphere();
		sub_geometry_buffer->computeVertexNormals();
		return sub_geometry_buffer;
	}
	void ThreeHelper::scaleAroundPivot(std::shared_ptr<threepp::BufferGeometry> geometry, const threepp::Vector3& pivot, const float& scaleX, const float& scaleY, const float& scaleZ)
	{
		threepp::Matrix4 translateToOrigin{ };
		translateToOrigin.makeTranslation(-pivot.x, -pivot.y, -pivot.z);
		threepp::Matrix4 scaleMatrix;
		scaleMatrix.makeScale(scaleX, scaleY, scaleZ);
		threepp::Matrix4 translateBack;
		translateBack.makeTranslation(pivot.x, pivot.y, pivot.z);
		threepp::Matrix4 matrix;
		matrix.multiplyMatrices(translateBack, scaleMatrix);
		matrix.multiplyMatrices(matrix, translateToOrigin);
		geometry->applyMatrix4(matrix);
	}

	std::shared_ptr<threepp::BufferGeometry> ThreeHelper::BuildSubGeometryWithOffset2(const std::map<int, std::vector<IFCModelCache::offset>>& view_geometries_offset_with_material,
		const std::vector<float>& vertices,
		const std::vector<float>& normals,
		const std::vector<unsigned int>& indices,
		const std::vector<std::shared_ptr<threepp::Material>>& materials,
		std::vector<std::shared_ptr<threepp::Material>>& new_materials)
	{
		std::shared_ptr<threepp::BufferGeometry> sub_geometry_buffer = threepp::BufferGeometry::create();
		std::vector<std::vector<float>> vecLstVertices{};
		std::vector<std::vector<unsigned int>> vecLstIndices{};
		std::vector<std::vector<float>> vecLstNormals{};
		int countVertices{ 0 };
		int countIndices{ 0 };
		int countNormals{ 0 };
		std::map<int, std::vector<std::pair<int, int>>> viewGeometriesWithMaterials{};

		uint32_t index_offset{ 0 };

		for (auto& [index_material, offsets] : view_geometries_offset_with_material)
		{
			for (const auto& offset : offsets)
			{
				std::vector<unsigned int> sub_indices{};
				const int range_index = offset.end_indices_offset - offset.begin_indices_offset + 1;
				sub_indices.reserve(range_index);
				sub_indices.insert(sub_indices.end(),
					indices.begin() + offset.begin_indices_offset,
					indices.begin() + offset.end_indices_offset + 1);
				unsigned int min_index = *std::min_element(sub_indices.begin(), sub_indices.end());
				std::vector<unsigned int> result(range_index);
				std::transform(sub_indices.begin(), sub_indices.end(), result.begin(),
					[min_index](unsigned int v) { return v - min_index; });
				vecLstIndices.emplace_back(result);
				uint32_t start = index_offset;
				uint32_t count = range_index;
				index_offset += range_index;
				std::pair<int, int> pair = { start,count };
				viewGeometriesWithMaterials[index_material].emplace_back(pair);
				countIndices += range_index;
				sub_indices.clear();
				const int range_vertex = offset.end_vertex_offset - offset.begin_vertex_offset + 1;
				std::vector<float> sub_vertex{};
				sub_vertex.reserve(range_vertex);
				sub_vertex.insert(sub_vertex.end(),
					vertices.begin() + offset.begin_vertex_offset,
					vertices.begin() + offset.end_vertex_offset + 1);
				vecLstVertices.emplace_back(sub_vertex);
				countVertices += range_vertex;
				std::vector<float> sub_normals{};
				sub_normals.reserve(range_vertex);
				sub_normals.insert(sub_normals.end(),
					normals.begin() + offset.begin_vertex_offset,
					normals.begin() + offset.end_vertex_offset + 1);
				vecLstNormals.emplace_back(sub_normals);
				countNormals += range_vertex;
			}
		}

		std::vector<float> mergeVetices{};
		std::vector<unsigned int> mergeIndices{};
		std::vector<float> mergeNormals{};
		mergeVetices.reserve(countVertices);
		mergeIndices.reserve(countIndices);
		mergeNormals.reserve(countNormals);

		/*MERGE VERTICES*/
		for (const auto& vecVertex : vecLstVertices)
		{
			mergeVetices.insert(mergeVetices.end(),
				vecVertex.begin(),
				vecVertex.end());
		}

		/*MERGE NORMALS*/
		for (const auto& vecNormal : vecLstNormals)
		{
			mergeNormals.insert(mergeNormals.end(),
				vecNormal.begin(),
				vecNormal.end());
		}

		/*MERGE INDICES*/
		int offset_indices{ 0 };
		size_t i = 0;
		for (const auto& vecIndices : vecLstIndices)
		{
			std::vector<unsigned int> result(vecIndices.size());
			std::transform(vecIndices.begin(), vecIndices.end(), result.begin(),
				[offset_indices](unsigned int v) { return v + offset_indices; });
			const int count = vecLstVertices[i].size() / 3;
			offset_indices += count;
			mergeIndices.insert(mergeIndices.end(),
				result.begin(),
				result.end());
			++i;
		}

		/*CREATE GROUP*/
		std::vector<threepp::GeometryGroup> groups{};
		groups.reserve(viewGeometriesWithMaterials.size());
		uint32_t start{ 0 };
		int new_mat_index{ 0 };
		for (auto& [materialIndex, geometry_indices] : viewGeometriesWithMaterials)
		{
			uint32_t count{ 0 };
			threepp::GeometryGroup group;
			group.start = start;
			group.materialIndex = new_mat_index;
			for (auto& geometry_index : geometry_indices)
			{
				count += geometry_index.second;
			}
			group.count = count;
			start += count;
			new_materials.emplace_back(materials[materialIndex]);
			groups.emplace_back(group);
			new_mat_index++;
		}

		sub_geometry_buffer->setIndex(mergeIndices);
		sub_geometry_buffer->setAttribute("position", threepp::FloatBufferAttribute::create(mergeVetices, 3));
		sub_geometry_buffer->setAttribute("normal", threepp::FloatBufferAttribute::create(mergeNormals, 3));
		sub_geometry_buffer->groups = groups;
		sub_geometry_buffer->computeBoundingBox();
		sub_geometry_buffer->computeBoundingSphere();
		sub_geometry_buffer->computeVertexNormals();
		return sub_geometry_buffer;
	}
}