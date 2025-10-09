#include <Cesium3DTilesSelection/TileContent.h>
#include <CesiumGeospatial/GlobeTransforms.h>
#include <CesiumGeometry/Transforms.h>
#include <Cesium3DTilesSelection/TilesetContentLoader.h>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include "MaplibrePrepareRendererResource.hpp"
#include <core/utils/CesiumHelper.hpp>
#include <core/convert/Tile.hpp>
#include "threepp/threepp.hpp"
#include <core/utils/ThreeHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <CesiumGeospatial/WebMercatorProjection.h>
#include <map/source/CustomRasterSource.hpp>
#include <cpr/cpr.h>
#include <format>
#include "MaplibreRasterOverlay.hpp"

#ifdef interface __STRUCT__
#undef interface __STRUCT__
#endif // interface __STRUCT__

namespace Cesium3DTilesSelection
{
    CesiumAsync::Future<TileLoadResultAndRenderResources> MaplibrePrepareRendererResource::prepareInLoadThread(
            const CesiumAsync::AsyncSystem& asyncSystem,
            TileLoadResult&& tileLoadResult,
            const glm::dmat4& transform,
            const Cesium3DTilesSelection::TileID& tileID,
            const std::any& rendererOptions)
    {
        mbgl::CanonicalTileID canonicalTileID{ 0,0,0 };
        mbgl::CanonicalTileID textureTileID{ 0,0,0 };
        std::optional<CesiumGeometry::QuadtreeTileID> geoTileID{ std::nullopt }; 
        if (tileID.index() == 1)
        {
            geoTileID = std::get<CesiumGeometry::QuadtreeTileID>(tileID);
        }
        if (tileID.index() == 3)
        {
            return asyncSystem.createResolvedFuture(TileLoadResultAndRenderResources{ std::move(tileLoadResult), nullptr });
        }
        CesiumGltf::Model* model_gltf = std::get_if<CesiumGltf::Model>(&tileLoadResult.contentKind);
        if (model_gltf == nullptr)
        {
            return asyncSystem.createResolvedFuture(TileLoadResultAndRenderResources{ std::move(tileLoadResult), nullptr });
        }
        return asyncSystem.createFuture<TileLoadResultAndRenderResources>([=, this,&canonicalTileID,&textureTileID](CesiumAsync::Promise<TileLoadResultAndRenderResources> p_promise) {
            glm::dmat4 tile_transform = transform;
            std::shared_ptr<threepp::Group> model_tile = createGroupThreeppFromModel(*model_gltf, tile_transform, canonicalTileID, textureTileID, geoTileID);
            model_tile->visible = false; 
            auto f = [this, canonicalTileID, textureTileID, model_tile]() {
                const std::string& token = "pk.eyJ1IjoiYW5odHVzeHl6IiwiYSI6ImNsdng4ZGp3ZTA2aDgyaWw3ZnM2NXJhcjcifQ.OV7YSJsVT8zY-L4tozXaVw";
                std::string formatUrl = std::format("https://api.mapbox.com/v4/mapbox.satellite/{}/{}/{}@2x.png?access_token={}", textureTileID.z, textureTileID.x, textureTileID.y, token);
                cpr::Response r;
                cpr::Header cprHeader;
                r = cpr::Get(cpr::Url{ formatUrl },
                    cprHeader);
                if (r.status_code == 200)
                {
                    if (!r.text.empty())
                    {
                        spdlog::info("Downloaded tile : {}, {}, {}", canonicalTileID.z, canonicalTileID.x, canonicalTileID.y);
                        auto img = mbgl::decodeImage(r.text);
                        size_t size = img.channels * img.size.width * img.size.height;
                        std::vector<unsigned char> vec_img(img.data.get(), img.data.get() + size);
                        threepp::Image three_img(std::move(vec_img),
                            img.size.width, img.size.height);
                        std::shared_ptr<threepp::Texture> texture = threepp::Texture::create(std::move(three_img));
                        if (img.channels == 4) {
                            texture->format = threepp::Format::RGBA;
                        }
                        else {
                            texture->format = threepp::Format::RGB;
                        }
                        texture->anisotropy = 16;
                        texture->wrapS = threepp::TextureWrapping::ClampToEdge;
                        texture->wrapT = threepp::TextureWrapping::ClampToEdge;
                        texture->minFilter = threepp::Filter::Linear;
                        texture->magFilter = threepp::Filter::Linear;
                        texture->generateMipmaps = true;
                        texture->needsUpdate();

                        model_tile->traverseType<threepp::Mesh>([&](threepp::Mesh& child) {
                            child.material()->as<threepp::MeshBasicMaterial>()->map = texture;
                            child.material()->needsUpdate();
                            });

                    }
                }
                else
                {
                    spdlog::error("Error code : {} with message : {}", r.status_code, r.error.message);
                }
            }; 
            f(); 
            p_promise.resolve(TileLoadResultAndRenderResources{
                std::move(tileLoadResult),
                new PrepareTileResult(model_tile, canonicalTileID,false)});
        }); 
    }

    void* MaplibrePrepareRendererResource::prepareInMainThread(
        Cesium3DTilesSelection::Tile& tile,
        void* pLoadThreadResult){
        auto tileId = tile.getTileID();
        const Cesium3DTilesSelection::TileContent& content = tile.getContent();
        const Cesium3DTilesSelection::TileRenderContent* pRenderContent = content.getRenderContent();
        if (pRenderContent == nullptr) {
            return pLoadThreadResult;
        }
        if (pLoadThreadResult) {
            PrepareTileResult* loadThreadResult = reinterpret_cast<PrepareTileResult*>(pLoadThreadResult);
            //create drawable here
            mbgl::CanonicalTileID canonicalTileID = loadThreadResult->canonicalTileID; 
            //spdlog::info("draw tile : {}, {} , {}", canonicalTileID.z, canonicalTileID.x, canonicalTileID.y);
            bool isDrawbleCreated{ false }; 
            mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(context.layerGroup);
            mbgl::gl::DrawableCustom* ptrDrawableCustom_{ nullptr }; 
            isDrawbleCreated = setDrawable.count(canonicalTileID) != 0; 
            if (!isDrawbleCreated)
            {
                context.interface->addCustomDrawableWithTile({ canonicalTileID.z, canonicalTileID.x, canonicalTileID.y }, ptrDrawableCustom_);
                setDrawable.insert({ canonicalTileID,ptrDrawableCustom_ }); 
            }
            else
            {
                ptrDrawableCustom_ = setDrawable.at(canonicalTileID); 
                if (canonicalTileID.z == 13)
                {
                    spdlog::info("tile ID : {}, {}, {}", canonicalTileID.z, canonicalTileID.x, canonicalTileID.y);
                }
            }

            if (ptrDrawableCustom_)
            {
                auto impl = ptrDrawableCustom_->getImpl();
                if (impl->scene)
                {
                    impl->scene->add(loadThreadResult->obj);
                    loadThreadResult->scene = impl->scene.get(); 
                }
            }
        }
        return pLoadThreadResult;
    }

    void MaplibrePrepareRendererResource::free(
        Cesium3DTilesSelection::Tile& tile,
        void* pLoadThreadResult,
        void* pMainThreadResult) noexcept {
        if (!pMainThreadResult)
            return; 
        PrepareTileResult* mainThreadResult = reinterpret_cast<PrepareTileResult*>(pMainThreadResult);
        if (mainThreadResult->obj && mainThreadResult->scene)
        {
            mainThreadResult->scene->remove(*mainThreadResult->obj);
            mainThreadResult->obj.reset();
        }
        delete mainThreadResult;
        mainThreadResult = nullptr;
    }

    void* MaplibrePrepareRendererResource::prepareRasterInLoadThread(CesiumGltf::ImageAsset& image, const std::any& rendererOptions)
    {
        return new PrepareRasterResult(nullptr);
    }

    void MaplibrePrepareRendererResource::attachRasterInMainThread(const Cesium3DTilesSelection::Tile& tile,
        int32_t overlayTextureCoordinateID,
        const CesiumRasterOverlays::RasterOverlayTile& rasterTile, 
        void* pMainThreadRendererResources, const glm::dvec2& translation,
        const glm::dvec2& scale)
    {
      
    }

    std::shared_ptr<threepp::Group> MaplibrePrepareRendererResource::createGroupThreeppFromModel(CesiumGltf::Model& gltf_model,
        glm::dmat4& tiletransform, 
        mbgl::CanonicalTileID& canonicalTileID,
        mbgl::CanonicalTileID& textureTileID,
        std::optional<CesiumGeometry::QuadtreeTileID> geoTileID)
    {
        dragon::CesiumHelper::B3DMExtensions b3dm_extension;
        auto model_tile = dragon::CesiumHelper::createMesh(gltf_model, b3dm_extension);

        threepp::Box3 box;
        box.setFromObject(*model_tile);
        glm::dvec3 local_center(box.getCenter().x, box.getCenter().y, box.getCenter().z);

        glm::dmat4 tile_transform = tiletransform; 
        const CesiumGltf::Node& rootNode = gltf_model.nodes.at(0);
        const std::vector<double>& translationArray = rootNode.translation;
        const std::vector<double>& rotationArray = rootNode.rotation;
        const std::vector<double>& scaleArray = rootNode.scale;
        glm::dmat4 transformationMat = glm::make_mat4<double>(rootNode.matrix.data()); 
     
        glm::dquat glmRot;
        constexpr int32_t translationColumnIndex = 3;
        glmRot = glm::quat_cast(transformationMat);

        glm::dquat quat = glm::quat_cast(glm::dmat3(tile_transform));
        glm::dmat4 rtc_transform(1.0); 
        if (b3dm_extension.hasRTC)
        {
            rtc_transform = glm::dmat4(
                glm::dvec4(1.0, 0.0, 0.0, 0.0),
                glm::dvec4(0.0, 1.0, 0.0, 0.0),
                glm::dvec4(0.0, 0.0, 1.0, 0.0),
                glm::dvec4(b3dm_extension.rtcCenter.x, b3dm_extension.rtcCenter.y, b3dm_extension.rtcCenter.z, 1.0)
            );
        }
       
        tile_transform =  tile_transform * rtc_transform;

        auto gltfUpAxisIt = gltf_model.extras.find("gltfUpAxis");
        if (gltfUpAxisIt == gltf_model.extras.end()) {
            tile_transform = tile_transform * CesiumGeometry::Transforms::Y_UP_TO_Z_UP;
        }

        const CesiumUtility::JsonValue& gltfUpAxis = gltfUpAxisIt->second;
        int gltfUpAxisValue = static_cast<int>(gltfUpAxis.getSafeNumberOrDefault(1));
        if (gltfUpAxisValue == static_cast<int>(CesiumGeometry::Axis::X)) {
            tile_transform = tile_transform * CesiumGeometry::Transforms::X_UP_TO_Z_UP;
        }
        else if (gltfUpAxisValue == static_cast<int>(CesiumGeometry::Axis::Y)) {
            tile_transform = tile_transform * CesiumGeometry::Transforms::Y_UP_TO_Z_UP;
        }
        else if (gltfUpAxisValue == static_cast<int>(CesiumGeometry::Axis::Z)) {
            // No transform required
        }

        glm::dvec4 new_ecef_center = tile_transform * transformationMat * glm::dvec4(local_center, 1.0);
        std::optional<glm::dvec3> wgs84Rtc = dragon::CesiumHelper::ecefToWgs84(glm::dvec3(new_ecef_center));

        const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;

        Convert::TileCoord mecator_tile;  
        //mecator_tile = Convert::wgs84ToTile(wgs84Rtc.value().x, wgs84Rtc.value().y);
        //canonicalTileID = { (uint8_t)mecator_tile.tileZ, (uint32_t)mecator_tile.tileX, (uint32_t)mecator_tile.tileY };
        if (!geoTileID)
        {
            mecator_tile = Convert::wgs84ToTile(wgs84Rtc.value().x, wgs84Rtc.value().y);
            canonicalTileID = { (uint8_t)mecator_tile.tileZ, (uint32_t)mecator_tile.tileX, (uint32_t)mecator_tile.tileY };
        }
        else
        {
            Convert::TileCoord texture_tile;
            mecator_tile = Convert::wgs84ToTile(wgs84Rtc.value().x, wgs84Rtc.value().y,geoTileID.value().level);
            texture_tile = Convert::wgs84ToTile(wgs84Rtc.value().x, wgs84Rtc.value().y, geoTileID.value().level + 1);
            canonicalTileID = { (uint8_t)mecator_tile.tileZ, (uint32_t)mecator_tile.tileX, (uint32_t)mecator_tile.tileY };
            textureTileID = { (uint8_t)texture_tile.tileZ, (uint32_t)texture_tile.tileX, (uint32_t)texture_tile.tileY };
        }
        
        glm::dmat4 matrix_transform(1.0);
        glm::dmat4 translateToOriginMatrix = glm::translate(glm::dmat4(1.0), -local_center);
        //glm::dmat4 translateBackMatrix = glm::translate(glm::dmat4(1.0), glm::dvec3(mecator_tile.localCoord.x, mecator_tile.localCoord.y, wgs84Rtc.value().z));
        glm::dmat4 translateBackMatrix = glm::translate(glm::dmat4(1.0), glm::dvec3(new_ecef_center));

        glm::dvec3 oZ_ecef = glm::normalize(ellipsoid.geodeticSurfaceNormal(new_ecef_center)); // Up
        glm::dvec3 oX_ecef = glm::normalize(glm::cross(glm::dvec3(0, 0, 1), oZ_ecef));           // East
        glm::dvec3 oY_ecef = glm::cross(oZ_ecef, oX_ecef);                                       // North

        // 3 vector target trên Mercator
        glm::dvec3 oX_mercator = glm::dvec3(1, 0, 0);
        glm::dvec3 oY_mercator = glm::dvec3(0, 1, 0);
        glm::dvec3 oZ_mercator = glm::dvec3(0, 0, 1);

        // TÍNH CÁC GÓC giữa từng cặp vector:

        // Góc giữa X_ecef và X_mercator  
        double angle_X = std::acos(glm::clamp(glm::dot(oX_ecef, oX_mercator), -1.0, 1.0));
        double angle_X_degrees = glm::degrees(angle_X);

        // Góc giữa Y_ecef và Y_mercator
        double angle_Y = std::acos(glm::clamp(glm::dot(oY_ecef, oY_mercator), -1.0, 1.0));
        double angle_Y_degrees = glm::degrees(angle_Y);

        // Góc giữa Z_ecef và Z_mercator
        double angle_Z = std::acos(glm::clamp(glm::dot(oZ_ecef, oZ_mercator), -1.0, 1.0));
        double angle_Z_degrees = glm::degrees(angle_Z);

        glm::dmat3 ecefMatrix = glm::dmat3(oX_ecef, oY_ecef, oZ_ecef);
        glm::dmat3 rotationMatrix = glm::transpose(ecefMatrix);
        glm::dquat q1 = glm::quat_cast(rotationMatrix);
        glm::dmat4 matrixRotate(1.0);
        matrixRotate = glm::toMat4(q1);
        matrixRotate = /*matrixRotate **/ glm::toMat4(quat) * glm::toMat4(glmRot);

        double metersPerExtentUnit = dragon::CesiumHelper::getMetersPerExtentUnit(wgs84Rtc.value().y);
        double scaleZ = 1 / metersPerExtentUnit;

        glm::dmat4 scaleMatrix = glm::scale(glm::dmat4(1.0), glm::dvec3(1.0 * metersPerExtentUnit,
            -1.0 * metersPerExtentUnit,
            1.0));

        matrix_transform = translateBackMatrix /** scaleMatrix*/  * matrixRotate * CesiumGeometry::Transforms::Y_UP_TO_Z_UP * translateToOriginMatrix;

        threepp::Matrix4 tmat;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                tmat.elements[col * 4 + row] = static_cast<float>(matrix_transform[col][row]);
            }
        }

        //ep model nam phang

        model_tile->traverseType<threepp::Mesh>([&](threepp::Mesh& child) {
            auto geom = child.geometry();
            if (auto bufferGeom = std::dynamic_pointer_cast<threepp::BufferGeometry>(geom)) {
                auto posAttr = bufferGeom->getAttribute<float>("position");
                auto posUV = bufferGeom->getAttribute<float>("uv");

                if (!posAttr) return;

                // Tìm bounding box để normalize UV
                float minX = (std::numeric_limits<float>::max)();
                float maxX = (std::numeric_limits<float>::lowest)();
                float minY = (std::numeric_limits<float>::max)();
                float maxY = (std::numeric_limits<float>::lowest)();

                std::vector<threepp::Vector2> localCoords;
                localCoords.reserve(posAttr->count());

                for (size_t i = 0; i < posAttr->count(); i++) {
                    threepp::Vector3 v(
                        posAttr->getX(i),
                        posAttr->getY(i),
                        posAttr->getZ(i)
                    );
                    v.applyMatrix4(tmat);

                    std::optional<glm::dvec3> wgs84 = dragon::CesiumHelper::ecefToWgs84(
                        glm::dvec3(v.x, v.y, v.z));

                    auto t = Convert::wgs84ToLocalInTile(
                        wgs84.value().x, wgs84.value().y,
                        canonicalTileID.x,
                        canonicalTileID.y,
                        canonicalTileID.z);

                    // Update position
                    posAttr->setXYZ(i, t.localX, t.localY, wgs84.value().z);

                    // Lưu local coords và tìm bounds
                    localCoords.emplace_back(t.localX, t.localY);
                    minX = (std::min)(minX, (float)t.localX);
                    maxX = (std::max)(maxX, (float)t.localX);
                    minY = (std::min)(minY, (float)t.localY);
                    maxY = (std::max)(maxY, (float)t.localY);
                }

                posAttr->needsUpdate();

                // Tính UV dựa trên actual bounds của geometry
                if (!posUV) {
                    std::vector<float> uv;
                    uv.reserve(localCoords.size() * 2);

                    float rangeX = maxX - minX;
                    float rangeY = maxY - minY;

                    // Tránh chia cho 0
                    if (rangeX < 0.001f) rangeX = 8192.0f;
                    if (rangeY < 0.001f) rangeY = 8192.0f;

                    for (const auto& coord : localCoords) {
                        // Normalize về [0, 1] dựa trên bounds thực tế
                        float u = (coord.x - minX) / rangeX;
                        float v = ((coord.y - minY) / rangeY); // Flip Y

                        // Clamp để chắc chắn
                        u = std::clamp(u, 0.0f, 1.0f);
                        v = std::clamp(v, 0.0f, 1.0f);

                        uv.push_back(u);
                        uv.push_back(v);
                    }

                    bufferGeom->setAttribute("uv",
                        threepp::FloatBufferAttribute::create(uv, 2));
                }
            }
        child.matrix->identity();
        child.position.set(0, 0, 0);
        child.rotation.set(0, 0, 0);
        child.scale.set(1, 1, 1);
        child.matrixAutoUpdate = false;
    });
    model_tile->matrixAutoUpdate = false;
    return model_tile;
    }
}