#include <CesiumGltfReader/GltfReader.h>
#include <CesiumGeospatial/Cartographic.h>
#include <CesiumGeospatial/Ellipsoid.h>
#include <CesiumGeospatial/Cartographic.h>
#include <cmath>
#include "CesiumHelper.hpp"
#include "threepp/threepp.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
namespace dragon
{
	std::shared_ptr<threepp::Group> CesiumHelper::createGLB(std::vector<std::byte>& bytes)
	{
		auto container = threepp::Group::create();
		CesiumGltfReader::GltfReader gltfReader;
		auto modelReadResult = gltfReader.readGltf(bytes);
		CesiumGltf::Model& gltf = *modelReadResult.model;
		for (size_t meshIndex = 0; meshIndex < gltf.meshes.size(); ++meshIndex)
		{
			const auto& gltfMesh = gltf.meshes[meshIndex];
			std::vector<std::shared_ptr<threepp::BufferGeometry>> geos;
			std::vector<std::shared_ptr<threepp::Material>> mats;
			for (size_t primIndex = 0; primIndex < gltfMesh.primitives.size(); ++primIndex)
			{
				const auto& primitive = gltfMesh.primitives[primIndex];
				//create geo
				auto geometry = threepp::BufferGeometry::create();
				if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
					auto positionIt = primitive.attributes.find("POSITION");
					if (positionIt->second >= 0)
					{
						const CesiumGltf::Accessor& positionAccessor = gltf.accessors[size_t(positionIt->second)];
						const CesiumGltf::BufferView& positionBufferView = gltf.bufferViews[size_t(positionAccessor.bufferView)];
						const auto& posBuffer = gltf.buffers[positionBufferView.buffer];
						size_t posOffset = positionBufferView.byteOffset + positionAccessor.byteOffset;
						const float* posData = reinterpret_cast<const float*>(posBuffer.cesium.data.data() + posOffset);
						size_t posCount = positionAccessor.count;
						geometry->setAttribute("position", threepp::FloatBufferAttribute::create(std::vector<float>(posData, posData + posCount * 3), 3));
					}
				}
				if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
				{
					auto normalIt = primitive.attributes.find("NORMAL");
					if (normalIt->second >= 0)
					{
						const CesiumGltf::Accessor& normalAccessor = gltf.accessors[size_t(normalIt->second)];
						const CesiumGltf::BufferView& normalBufferView = gltf.bufferViews[size_t(normalAccessor.bufferView)];
						const auto& posBuffer = gltf.buffers[normalBufferView.buffer];
						size_t posOffset = normalBufferView.byteOffset + normalAccessor.byteOffset;
						const float* posData = reinterpret_cast<const float*>(posBuffer.cesium.data.data() + posOffset);
						size_t posCount = normalAccessor.count;
						geometry->setAttribute("normal", threepp::FloatBufferAttribute::create(std::vector<float>(posData, posData + posCount * 3), 3));
					}
				}
				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
				{
					int uvAccessorIndex = primitive.attributes.at("TEXCOORD_0");
					const CesiumGltf::Accessor& uvAccessor = gltf.accessors[uvAccessorIndex];
					const auto& uvBufferView = gltf.bufferViews[size_t(uvAccessor.bufferView)];
					const auto& uvBuffer = gltf.buffers[uvBufferView.buffer];
					size_t uvOffset = uvBufferView.byteOffset + uvAccessor.byteOffset;
					const float* uvData = reinterpret_cast<const float*>(uvBuffer.cesium.data.data() + uvOffset);
					size_t uvCount = uvAccessor.count;
					geometry->setAttribute("uv", threepp::FloatBufferAttribute::create(std::vector<float>(uvData, uvData + uvCount * 2), 2));
				}
				if (primitive.indices >= 0)
				{
					const CesiumGltf::Accessor& indicesAccessor = gltf.accessors[size_t(primitive.indices)];
					const CesiumGltf::BufferView& indicesBufferView = gltf.bufferViews[size_t(indicesAccessor.bufferView)];
					const auto& indexBuffer = gltf.buffers[indicesBufferView.buffer];
					size_t indexOffset = indicesBufferView.byteOffset + indicesAccessor.byteOffset;
					if (indicesAccessor.componentType == CesiumGltf::Accessor::ComponentType::UNSIGNED_SHORT)
					{
						const uint16_t* indexData = reinterpret_cast<const uint16_t*>(indexBuffer.cesium.data.data() + indexOffset);
						std::vector<uint32_t> indices32(indexData, indexData + indicesAccessor.count);
						geometry->setIndex(std::move(indices32));
					}
					else if (indicesAccessor.componentType == CesiumGltf::Accessor::ComponentType::UNSIGNED_INT)
					{
						const uint32_t* indexData = reinterpret_cast<const uint32_t*>(indexBuffer.cesium.data.data() + indexOffset);
						std::vector<uint32_t> indices32(indexData, indexData + indicesAccessor.count);
						geometry->setIndex(std::move(indices32));
					}

				}

				auto material = threepp::MeshBasicMaterial::create();
				material->side = threepp::Side::Double;
				int matIndex = primitive.material;
				const auto& gltfMaterial = gltf.materials[matIndex];
				if (gltfMaterial.pbrMetallicRoughness.has_value())
				{
					const auto& pbr = gltfMaterial.pbrMetallicRoughness.value();
					if (pbr.baseColorTexture.has_value())
					{
						int texIndex = pbr.baseColorTexture.value().index;
						const auto& gltfTexture = gltf.textures[texIndex];
						const auto& gltfImage = gltf.images[size_t(gltfTexture.source)];
						CesiumGltf::ImageAsset& image = *gltfImage.pAsset;
						if (image.pixelData.size() > 0)
						{
							//create image
							auto size = image.pixelData.size();
							std::vector<unsigned char> ucharBuffer(size);
							std::memcpy(ucharBuffer.data(), image.pixelData.data(), image.pixelData.size());
							threepp::Image three_img(ucharBuffer,
								image.width,
								image.height);
							auto texture = threepp::Texture::create(three_img);
							if (image.channels == 4)
							{
								texture->format = threepp::Format::RGBA;
							}
							else
							{
								texture->format = threepp::Format::RGB;
							}
							texture->needsUpdate();
							material->as<threepp::MeshBasicMaterial>()->map = texture;
						}
					}
				}
				mats.emplace_back(material);
				geos.emplace_back(geometry);
			}
			auto mergeo = threepp::mergeBufferGeometries(geos, true);
			auto threepp_mesh = threepp::Mesh::create(mergeo, mats);
			container->add(threepp_mesh);
		}
		return container;
	}
}