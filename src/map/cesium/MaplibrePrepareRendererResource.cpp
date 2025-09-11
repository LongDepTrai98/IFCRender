#include <Cesium3DTilesSelection/TileContent.h>
#include <CesiumGeospatial/GlobeTransforms.h>
#include <CesiumGeometry/Transforms.h>
#include <Cesium3DTilesSelection/TilesetContentLoader.h>
#include "MaplibrePrepareRendererResource.hpp"
#include <core/utils/CesiumHelper.hpp>
#include <core/convert/Tile.hpp>
#include "threepp/threepp.hpp"
#include <core/utils/ThreeHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <thread>
namespace Cesium3DTilesSelection
{
    CesiumAsync::Future<TileLoadResultAndRenderResources> MaplibrePrepareRendererResource::prepareInLoadThread(
            const CesiumAsync::AsyncSystem& asyncSystem,
            TileLoadResult&& tileLoadResult,
            const glm::dmat4& transform,
            const std::any& rendererOptions)
    {
        prepareInLoadThreadTestCallback(tileLoadResult);
        CesiumGltf::Model* model_gltf = std::get_if<CesiumGltf::Model>(&tileLoadResult.contentKind);
        if (model_gltf == nullptr)
        {
            return asyncSystem.createResolvedFuture(TileLoadResultAndRenderResources{ std::move(tileLoadResult), nullptr });
        }
        glm::dmat4 tile_transform = transform; 
        std::shared_ptr<threepp::Group> model_tile = createGroupThreeppFromModel(*model_gltf, tile_transform);
        return asyncSystem.createResolvedFuture(TileLoadResultAndRenderResources{
            std::move(tileLoadResult),
            new PrepareResult(model_tile)});
    }

    void* MaplibrePrepareRendererResource::prepareInMainThread(
        Cesium3DTilesSelection::Tile& tile,
        void* pLoadThreadResult){
        const Cesium3DTilesSelection::TileContent& content = tile.getContent();
        const Cesium3DTilesSelection::TileRenderContent* pRenderContent = content.getRenderContent();
        std::cout << "prepare" << std::endl; 
        if (pRenderContent == nullptr) {
            return pLoadThreadResult;
        }
        if (pLoadThreadResult) {
            PrepareResult* ptr_model = reinterpret_cast<PrepareResult*>(pLoadThreadResult);
            if (ptr_model->obj)
            {
                context.scene->add(ptr_model->obj);
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
        PrepareResult* ptr_model = reinterpret_cast<PrepareResult*>(pMainThreadResult);
        if (ptr_model->obj)
        {
            context.scene->remove(*ptr_model->obj); 
        }
        delete pMainThreadResult; 
        pMainThreadResult = nullptr; 
    }
    std::shared_ptr<threepp::Group> MaplibrePrepareRendererResource::createGroupThreeppFromModel(Cesium3DTilesSelection::Tile& tile)
    {
        std::string tile_str_id = std::get<std::string>(tile.getTileID());
        Cesium3DTilesSelection::TileRenderContent* renderContent = tile.getContent().getRenderContent();
        CesiumGltf::Model gltf_model = renderContent->getModel();
        glm::dmat4 tile_transform = tile.getTransform(); 
        std::shared_ptr<threepp::Group> model_tile = createGroupThreeppFromModel(gltf_model, tile_transform);
        model_tile->name = tile_str_id; 
        return model_tile; 
    }
    std::shared_ptr<threepp::Group> MaplibrePrepareRendererResource::createGroupThreeppFromModel(CesiumGltf::Model& gltf_model,
        glm::dmat4& tile_transform)
    {
        dragon::CesiumHelper::B3DMExtensions b3dm_extension;
        auto model_tile = dragon::CesiumHelper::createMesh(gltf_model, b3dm_extension);
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
        threepp::Box3 box;
        box.setFromObject(*model_tile);
        glm::dvec3 local_center(box.getCenter().x, box.getCenter().y, box.getCenter().z);

        tile_transform = tile_transform * rtc_transform;
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

        //glm::dvec3 new_center = glm::dvec3(box.getCenter().x,box.getCenter().y,box.getCenter().z) + b3dm_extension.rtcCenter;
        glm::dvec4 new_ecef_center = tile_transform * glm::vec4(local_center, 1.0);
        std::optional<glm::dvec3> wgs84Rtc = dragon::CesiumHelper::ecefToWgs84(glm::dvec3(new_ecef_center));

        const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
        auto mecator_tile = Convert::wgs84ToLocalInTile(wgs84Rtc.value().x, wgs84Rtc.value().y, context.root_tile_id.x,
            context.root_tile_id.y);

        glm::dmat4 matrix_transform(1.0);
        glm::dmat4 translateToOriginMatrix = glm::translate(glm::dmat4(1.0), -local_center);
        glm::dmat4 translateBackMatrix = glm::translate(glm::dmat4(1.0), glm::dvec3(mecator_tile.localX, mecator_tile.localY, wgs84Rtc.value().z));

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
        matrixRotate = matrixRotate * glm::toMat4(quat); 
        //// HOẶC tính góc Euler (roll, pitch, yaw)
        //glm::dvec3 eulerAngles = glm::eulerAngles(q1);
        //glm::dmat4 matrixRotate(1.0);

        //if (eulerAngles.z != 0.0) {
        //    matrixRotate = glm::rotate(matrixRotate, eulerAngles.z, glm::dvec3(0.0, 0.0, 1.0));
        //}
        //if (eulerAngles.y != 0.0) {
        //    matrixRotate = glm::rotate(matrixRotate, eulerAngles.y, glm::dvec3(0.0, 1.0, 0.0));
        //}
        //if (eulerAngles.x != 0.0) {
        //    matrixRotate = glm::rotate(matrixRotate, eulerAngles.x, glm::dvec3(1.0, 0.0, 0.0));
        //}

        double metersPerExtentUnit = dragon::CesiumHelper::getMetersPerExtentUnit(wgs84Rtc.value().y);
        double scaleZ = 1 / metersPerExtentUnit;

        glm::dmat4 scaleMatrix = glm::scale(glm::dmat4(1.0), glm::dvec3(1.0 * metersPerExtentUnit,
            -1.0 * metersPerExtentUnit,
            1.0));

        matrix_transform = translateBackMatrix * scaleMatrix * matrixRotate * CesiumGeometry::Transforms::Y_UP_TO_Z_UP * translateToOriginMatrix;

        threepp::Matrix4 tmat;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                tmat.elements[col * 4 + row] = static_cast<float>(matrix_transform[col][row]);
            }
        }

        model_tile->traverseType<threepp::Mesh>([&](threepp::Mesh& child) {
            child.matrix->identity();
            child.position.set(0, 0, 0);
            child.rotation.set(0, 0, 0);
            child.scale.set(1, 1, 1);
            child.matrixAutoUpdate = false;
            child.applyMatrix4(tmat);
        });

        model_tile->matrixAutoUpdate = false;
        return model_tile;
    }
}