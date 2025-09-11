#include <CesiumGltfReader/GltfReader.h>
#include <CesiumGeospatial/Cartographic.h>
#include <CesiumGeospatial/Ellipsoid.h>
#include <CesiumGeospatial/BoundingRegion.h>
#include <CesiumGeospatial/GlobeRectangle.h>
#include <CesiumGeospatial/S2CellBoundingVolume.h>
#include <CesiumGeospatial/GlobeTransforms.h>
#include <CesiumGeospatial/LocalHorizontalCoordinateSystem.h>
#include <CesiumGltf/ExtensionCesiumRTC.h>
#include <CesiumGltf/BufferCesium.h>
#include <mbgl/util/projection.hpp>
#include <mbgl/util/constants.hpp>
#include <cmath>
#include "CesiumHelper.hpp"
#include "ThreeHelper.hpp"
#include "threepp/threepp.hpp"
#include "threepp/math/Euler.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include <core/convert/Tile.hpp>
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
	std::shared_ptr<threepp::Group> CesiumHelper::createMesh(CesiumGltf::Model& gltf, B3DMExtensions& extension)
	{
		auto container = threepp::Group::create();
		bool hasRTC = false; 
		if (gltf.extensionsUsed.size() > 0) {
			for (const auto& ext : gltf.extensionsUsed) {
				if (ext == "CESIUM_RTC") {
					extension.hasRTC = true; 
					const CesiumGltf:: ExtensionCesiumRTC* cesiumRTC =
						gltf.getExtension<CesiumGltf::ExtensionCesiumRTC>();
					extension.rtcCenter = glm::dvec3(cesiumRTC->center[0],cesiumRTC->center[1],cesiumRTC->center[2]);
				}
			}
		}

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
						CesiumGltf::BufferView& positionBufferView = gltf.bufferViews[size_t(positionAccessor.bufferView)];
						auto& posBuffer = gltf.buffers[positionBufferView.buffer];
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
	glm::dvec3 CesiumHelper::wgs84ToEcef(const double& lon, const double& lat, const double& height)
	{
		const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
		CesiumGeospatial::Cartographic wgs_coord_deg = CesiumGeospatial::Cartographic::fromDegrees(lon,lat,height);
		return ellipsoid.cartographicToCartesian(wgs_coord_deg);
	}
	std::optional<glm::dvec3> CesiumHelper::ecefToWgs84(glm::dvec3 ecef)
	{
		const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
		std::optional<CesiumGeospatial::Cartographic> cart = ellipsoid.cartesianToCartographic(ecef);
		if (!cart) return std::nullopt; 
		return glm::dvec3(CesiumUtility::Math::radiansToDegrees(cart.value().longitude),
			CesiumUtility::Math::radiansToDegrees(cart.value().latitude), 
			cart.value().height);
	}
	glm::dvec3 CesiumHelper::getCenterBoundingVolume(const Cesium3DTilesSelection::BoundingVolume& BoundingVolume)
	{
		return std::visit(
			[](auto&& arg) -> glm::dvec3 {
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, CesiumGeometry::BoundingSphere>) {
					return arg.getCenter();
				}
				else if constexpr (std::is_same_v<T, CesiumGeometry::OrientedBoundingBox>) {
					return arg.getCenter();
				}
				else if constexpr (std::is_same_v<T, CesiumGeospatial::BoundingRegion>) {
					return arg.getBoundingBox().getCenter();
				}
				//else if constexpr (std::is_same_v<T, CesiumGeospatial::BoundingRegionWithLooseFittingHeights>) {
				//	const auto& rect = arg.getBoundingRegion().getRectangle();
				//	double lon = (rect.west + rect.east) * 0.5;
				//	double lat = (rect.south + rect.north) * 0.5;
				//	double h = (arg.getBoundingRegion().getMinimumHeight() +
				//		arg.getBoundingRegion().getMaximumHeight()) * 0.5;
				//	CesiumGeospatial::Cartographic c(lon, lat, h);
				//	return CesiumGeospatial::Ellipsoid::WGS84.cartographicToCartesian(c);
				//}
				//else if constexpr (std::is_same_v<T, CesiumGeospatial::S2CellBoundingVolume>) {
				//	// Lấy tâm của S2 cell
				//	CesiumGeospatial::Cartographic c = arg.getBoundingRectangle().computeCenter();
				//	return CesiumGeospatial::Ellipsoid::WGS84.cartographicToCartesian(c);
				//}
				//else if constexpr (std::is_same_v<T, CesiumGeometry::BoundingCylinderRegion>) {
				//	const auto& rect = arg.getRectangle();
				//	double lon = (rect.west + rect.east) * 0.5;
				//	double lat = (rect.south + rect.north) * 0.5;
				//	double h = (arg.getMinimumHeight() + arg.getMaximumHeight()) * 0.5;
				//	CesiumGeospatial::Cartographic c(lon, lat, h);
				//	return CesiumGeospatial::Ellipsoid::WGS84.cartographicToCartesian(c);
				//}
				else {
					// fallback
					return glm::dvec3(0.0);
				}
			},
			BoundingVolume
		);
	}

	double CesiumHelper::calculateECEFMercatorTilt(const glm::dvec3& ecefPoint)
	{
		auto cartographic = CesiumGeospatial::Ellipsoid::WGS84.cartesianToCartographic(ecefPoint);
		if (!cartographic) return 0.0;

		double latitude = cartographic->latitude;

		double mercatorConvergence = latitude; // đơn giản hóa

		glm::dvec3 surfaceNormal = CesiumGeospatial::Ellipsoid::WGS84.geodeticSurfaceNormal(ecefPoint);
		glm::dvec3 radialDirection = glm::normalize(ecefPoint);

		double angleDiff = acos(glm::dot(surfaceNormal, radialDirection));

		return glm::degrees(angleDiff);
	}
	
	double CesiumHelper::getMetersPerExtentUnit(double lat)
	{
		auto scale_512 = 1 / mbgl::Projection::getMetersPerPixelAtLatitude(lat, 16);
		return (scale_512 * (double)mbgl::util::EXTENT) * 1 / mbgl::util::tileSize_D; 
	}

	glm::dmat4 CesiumHelper::createMatrixRotateAroundPivot(const glm::dvec3& pivot, double rotateX, double rotateY, double rotateZ)
	{
		glm::dmat4 result(1.0); // identity
		glm::dmat4 translateToOrigin = glm::translate(glm::dmat4(1.0), -pivot);
		glm::dmat4 rotation(1.0);
		if (rotateZ != 0.0) {
			rotation = glm::rotate(rotation, rotateZ, glm::dvec3(0.0, 0.0, 1.0));
		}
		if (rotateY != 0.0) {
			rotation = glm::rotate(rotation, rotateY, glm::dvec3(0.0, 1.0, 0.0));
		}
		if (rotateX != 0.0) {
			rotation = glm::rotate(rotation, rotateX, glm::dvec3(1.0, 0.0, 0.0));
		}
		glm::dmat4 translateBack = glm::translate(glm::dmat4(1.0), pivot);
		result = translateBack * rotation * translateToOrigin;
		return result;
	}

	glm::dmat4 CesiumHelper::createMatrixScaleAroundPivot(const glm::dvec3& pivot, double scaleX, double scaleY, double scaleZ)
	{
		glm::dmat4 translateToOrigin = glm::translate(glm::dmat4(1.0), -pivot);

		glm::dmat4 scaleMatrix = glm::scale(glm::dmat4(1.0), glm::dvec3(scaleX, scaleY, scaleZ));

		glm::dmat4 translateBack = glm::translate(glm::dmat4(1.0), pivot);

		return translateBack * scaleMatrix * translateToOrigin;
	}

	glm::dmat4 CesiumHelper::createMatrixTranslateAroundPivot(const glm::dvec3& pivot, double tar_x, double tar_y, double tar_z)
	{
		double deltaX = tar_x - pivot.x;
		double deltaY = tar_y - pivot.y;
		double deltaZ = tar_z - pivot.z;
		glm::dmat4 result = glm::translate(glm::dmat4(1.0), glm::dvec3(deltaX, deltaY, deltaZ));
		return result;
	}

	std::shared_ptr<threepp::Mesh> CesiumHelper::createOrientedBoundingBox(Cesium3DTilesSelection::BoundingVolume& boundingVolume, const mbgl::CanonicalTileID& tileID)
	{
		return std::visit(
			[tileID](auto&& arg) -> std::shared_ptr<threepp::Mesh> {
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, CesiumGeometry::BoundingSphere>) {
					return nullptr;
				}
				else if constexpr (std::is_same_v<T, CesiumGeometry::OrientedBoundingBox>) {
					return nullptr;
				}
				else if constexpr (std::is_same_v<T, CesiumGeospatial::BoundingRegion>) {
					const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
					const CesiumGeometry::OrientedBoundingBox& orientedBoundingBox = arg.getBoundingBox();

					CesiumGeospatial::GlobeRectangle globalRectangle = arg.getRectangle();

					double _wgs84West = CesiumUtility::Math::radiansToDegrees(globalRectangle.getWest());
					double _wgs84South = CesiumUtility::Math::radiansToDegrees(globalRectangle.getSouth());
					double _wgs84East = CesiumUtility::Math::radiansToDegrees(globalRectangle.getEast());
					double _wgs84North = CesiumUtility::Math::radiansToDegrees(globalRectangle.getNorth());

					double _minimumHeight = arg.getMinimumHeight();
					double _maximumHeight = arg.getMaximumHeight();

					//create 8 corner
					std::vector<glm::dvec3> corners;

					// Bottom corners (minimum height)
					corners.push_back({ _wgs84West, _wgs84South, _minimumHeight });
					corners.push_back({_wgs84East, _wgs84South, _minimumHeight});
					corners.push_back({ _wgs84East, _wgs84North, _minimumHeight});
					corners.push_back({ _wgs84West, _wgs84North, _minimumHeight });

					// Top corners (maximum height)
					corners.push_back({ _wgs84West, _wgs84South, _maximumHeight });
					corners.push_back({ _wgs84East, _wgs84South, _maximumHeight });
					corners.push_back({ _wgs84East, _wgs84North, _maximumHeight });
					corners.push_back({ _wgs84West, _wgs84North, _maximumHeight });

					std::vector<float> vertices; 
					vertices.reserve(24); 

					for (auto& corner : corners)
					{
						Convert::LocalCoord localCoord = Convert::wgs84ToLocalInTile(corner.x, corner.y,tileID.x,tileID.y);
						vertices.emplace_back(localCoord.localX); 
						vertices.emplace_back(localCoord.localY); 
						vertices.emplace_back(corner.z); 
					}

					std::vector<unsigned int> indices = {
						// Bottom face
						0, 1, 2,    2, 3, 0,
						// Top face  
						4, 7, 6,    6, 5, 4,
						// South face (front)
						0, 4, 5,    5, 1, 0,
						// North face (back)
						2, 6, 7,    7, 3, 2,
						// West face (left)
						0, 3, 7,    7, 4, 0,
						// East face (right)
						1, 5, 6,    6, 2, 1
					};

					std::shared_ptr<threepp::BufferGeometry> buff_geometry = threepp::BufferGeometry::create();

					threepp::Box3 box; 
					box.setFromArray(vertices); 

					auto center = box.getCenter(); 

					buff_geometry->setIndex(indices); 
					buff_geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3)); 
					buff_geometry->computeVertexNormals(); 


					double scaleZ = Convert::computeScaleZForLevel(tileID.z);
					double center_lat = (_wgs84South + _wgs84North) * 0.5; 
					double metersPerExtentUnit = getMetersPerExtentUnit(center_lat);
					threepp::Matrix4 matrix_scale = ThreeHelper::createMatrixScaleAroundPivot(center,
						1.0/* * metersPerExtentUnit*/,
						1.0 /** metersPerExtentUnit*/,
						1.0 /** scaleZ*/ /** metersPerExtentUnit*/);
					//buff_geometry->applyMatrix4(matrix_scale);
					std::shared_ptr<threepp::MeshBasicMaterial> box_mat = threepp::MeshBasicMaterial::create();
					box_mat->color = threepp::Color::red;
					box_mat->wireframe = true; 
					return threepp::Mesh::create(buff_geometry,box_mat); 
					//glm::dvec3 ecef_center_box = orientedBoundingBox.getCenter();
					//glm::dvec3 worldUp = ellipsoid.geodeticSurfaceNormal(ecef_center_box);
					//double angle = std::acos(glm::dot(worldUp, glm::dvec3(0, 0, 1)));
					////truc quay 
					//glm::dvec3 axis = glm::normalize(glm::cross(worldUp, glm::dvec3(0, 0, 1))); 
					//glm::dquat q = glm::angleAxis(angle, axis);
					//threepp::Quaternion tq(
					//	static_cast<float>(q.x),
					//	static_cast<float>(q.y),
					//	static_cast<float>(q.z),
					//	static_cast<float>(q.w)
					//);
				
					//threepp::Matrix4 R;
					//R.makeRotationFromQuaternion(tq);

					//threepp::Euler euler; 
					//euler.setFromQuaternion(tq); 
					//double eulerX = threepp::math::radToDeg(euler.x); 
					//double eulerY = threepp::math::radToDeg(euler.y);
					//double eulerZ = threepp::math::radToDeg(euler.z);

					//std::optional<glm::dvec3> wgs84_center_box = ecefToWgs84(ecef_center_box); 
					////Convert::TileCoord tile = Convert::wgs84ToTile(wgs84_center_box.value().x, wgs84_center_box.value().y);
					//Convert::LocalCoord coord_center = Convert::wgs84ToLocalInTile(wgs84_center_box.value().x, wgs84_center_box.value().y, tileID.x, tileID.y);
					//glm::dmat3 halfAxes = orientedBoundingBox.getHalfAxes(); 
					//std::vector<float> vertices; 
					//vertices.reserve(24); 
					//glm::dvec3 unitVertices[8] = {
					//	glm::dvec3(-1, -1, -1),  // 0
					//	glm::dvec3(1, -1, -1),  // 1
					//	glm::dvec3(1,  1, -1),  // 2
					//	glm::dvec3(-1,  1, -1),  // 3
					//	glm::dvec3(-1, -1,  1),  // 4
					//	glm::dvec3(1, -1,  1),  // 5
					//	glm::dvec3(1,  1,  1),  // 6
					//	glm::dvec3(-1,  1,  1)   // 7
					//};
					//for (int i = 0; i < 8; i++) {
					//	glm::dvec3 vertex = glm::dvec3(coord_center.localX, coord_center.localY, wgs84_center_box.value().z) + halfAxes * unitVertices[i];
					//	vertices.emplace_back(vertex.x);
					//	vertices.emplace_back(vertex.y);
					//	vertices.emplace_back(vertex.z);
					//}
					//std::vector<unsigned int> indices = {
					//	0, 1, 2,    2, 3, 0,
					//	4, 7, 6,    6, 5, 4,
					//	0, 4, 5,    5, 1, 0,
					//	2, 6, 7,    7, 3, 2,
					//	0, 3, 7,    7, 4, 0,
					//	1, 5, 6,    6, 2, 1
					//};
					//std::shared_ptr<threepp::BufferGeometry> buff_geometry = threepp::BufferGeometry::create(); 
					//buff_geometry->setIndex(indices); 
					//buff_geometry->setAttribute("position", threepp::FloatBufferAttribute::create(vertices, 3)); 
					//buff_geometry->computeVertexNormals();

					////calculate scale 
					//double scaleZ = Convert::computeScaleZForLevel(tileID.z);
					//double metersPerExtentUnit = getMetersPerExtentUnit(wgs84_center_box.value().y); 
					//threepp::Matrix4 matrix_scale = ThreeHelper::createMatrixScaleAroundPivot(threepp::Vector3(static_cast<float>(coord_center.localX), 
					//	static_cast<float>(coord_center.localY),
					//	static_cast<float>(wgs84_center_box.value().z)),
					//	1.0 * metersPerExtentUnit,
					//	1.0 * metersPerExtentUnit,
					//	1.0 * scaleZ * metersPerExtentUnit);
					//threepp::Matrix4 matrix_rotate = ThreeHelper::createMatrixRotateAroundPivot(threepp::Vector3(static_cast<float>(coord_center.localX),
					//	static_cast<float>(coord_center.localY),
					//	static_cast<float>(wgs84_center_box.value().z)),
					//	euler.x,
					//	euler.y,
					//	euler.z); 
					//buff_geometry->applyMatrix4(matrix_rotate); 
					//buff_geometry->applyMatrix4(matrix_scale);

					//std::shared_ptr<threepp::MeshBasicMaterial> box_mat = threepp::MeshBasicMaterial::create(); 
					//box_mat->color = threepp::Color::red; 
					////box_mat->wireframe = true; 
					////box_mat->clipping = true; 
					////std::shared_ptr<threepp::WireframeGeometry> wire_frame = threepp::WireframeGeometry::create(*buff_geometry); 
					//return threepp::Mesh::create(buff_geometry,box_mat);
					//return nullptr; 
				}
				//else if constexpr (std::is_same_v<T, CesiumGeospatial::BoundingRegionWithLooseFittingHeights>) {
				//	const auto& rect = arg.getBoundingRegion().getRectangle();
				//	double lon = (rect.west + rect.east) * 0.5;
				//	double lat = (rect.south + rect.north) * 0.5;
				//	double h = (arg.getBoundingRegion().getMinimumHeight() +
				//		arg.getBoundingRegion().getMaximumHeight()) * 0.5;
				//	CesiumGeospatial::Cartographic c(lon, lat, h);
				//	return CesiumGeospatial::Ellipsoid::WGS84.cartographicToCartesian(c);
				//}
				//else if constexpr (std::is_same_v<T, CesiumGeospatial::S2CellBoundingVolume>) {
				//	// Lấy tâm của S2 cell
				//	CesiumGeospatial::Cartographic c = arg.getBoundingRectangle().computeCenter();
				//	return CesiumGeospatial::Ellipsoid::WGS84.cartographicToCartesian(c);
				//}
				//else if constexpr (std::is_same_v<T, CesiumGeometry::BoundingCylinderRegion>) {
				//	const auto& rect = arg.getRectangle();
				//	double lon = (rect.west + rect.east) * 0.5;
				//	double lat = (rect.south + rect.north) * 0.5;
				//	double h = (arg.getMinimumHeight() + arg.getMaximumHeight()) * 0.5;
				//	CesiumGeospatial::Cartographic c(lon, lat, h);
				//	return CesiumGeospatial::Ellipsoid::WGS84.cartographicToCartesian(c);
				//}
				else {
					// fallback
					return nullptr;
				}
			},
			boundingVolume
		);
	}
}