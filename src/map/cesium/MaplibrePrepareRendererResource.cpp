#include <Cesium3DTilesSelection/TileContent.h>
#include <CesiumGeospatial/GlobeTransforms.h>
#include <CesiumGeometry/Transforms.h>
#include "MaplibrePrepareRendererResource.hpp"
#include <core/utils/CesiumHelper.hpp>
#include <core/convert/Tile.hpp>
#include "threepp/threepp.hpp"
#include <core/utils/ThreeHelper.hpp>
namespace Cesium3DTilesSelection
{
    CesiumAsync::Future<TileLoadResultAndRenderResources> MaplibrePrepareRendererResource::prepareInLoadThread(
            const CesiumAsync::AsyncSystem& asyncSystem,
            TileLoadResult&& tileLoadResult,
            const glm::dmat4& transform,
            const std::any& rendererOptions)
    {
        prepareInLoadThreadTestCallback(tileLoadResult);
        return asyncSystem.createResolvedFuture(TileLoadResultAndRenderResources{
            std::move(tileLoadResult),
            new AllocationResult{totalAllocation} });
    }

    void* MaplibrePrepareRendererResource::prepareInMainThread(
        Cesium3DTilesSelection::Tile& tile,
        void* pLoadThreadResult){
        if (pLoadThreadResult) {
            spdlog::info("Prepare in main thread Tile id : {}", std::get<std::string>(tile.getTileID()));
            AllocationResult* loadThreadResult =
                reinterpret_cast<AllocationResult*>(pLoadThreadResult);
            //create new bounding box 
            std::string tile_str_id = std::get<std::string>(tile.getTileID());

            if (t_count == 0)
            {
                Cesium3DTilesSelection::BoundingVolume bouding_tile_voulume = tile.getBoundingVolume();
                Cesium3DTilesSelection::TileRenderContent* renderContent = tile.getContent().getRenderContent();
                CesiumGltf::Model model = renderContent->getModel();
                dragon::CesiumHelper::B3DMExtensions extension;
                auto tmp = dragon::CesiumHelper::createMesh(model, extension);
                auto t = std::get<CesiumGeometry::BoundingSphere>(bouding_tile_voulume); 

                std::shared_ptr<threepp::Mesh> orientedBoundingBox = dragon::CesiumHelper::createOrientedBoundingBox(bouding_tile_voulume, root_tile_id);
                extension.rtcCenter = t.getCenter(); 
                if(orientedBoundingBox) orientedBoundingBox->name = tile_str_id;

                if (extension.hasRTC)
                {
                    const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
                    std::optional<glm::dvec3> wgs84Rtc = dragon::CesiumHelper::ecefToWgs84(extension.rtcCenter);
                    auto tile = Convert::wgs84ToLocalInTile(wgs84Rtc.value().x, wgs84Rtc.value().y, root_tile_id.x, root_tile_id.y); 
                    threepp::Box3 box;
                    box.setFromObject(*tmp);
                    auto center = box.getCenter(); 
                    glm::dvec3 dcenter = glm::dvec3(center.x, center.y, center.z);

                    glm::dmat4 matrix_transform(1.0);
                    glm::dmat4 translateToOriginMatrix = glm::translate(glm::dmat4(1.0), -dcenter);

                    glm::dvec3 oZ_ecef = glm::normalize(ellipsoid.geodeticSurfaceNormal(extension.rtcCenter)); // Up
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

                    double totalRotationAngle = 2.0 * std::acos(glm::clamp(std::abs(q1.w), 0.0, 1.0));
                    double totalRotationAngle_degrees = glm::degrees(totalRotationAngle);

                    // HOẶC tính góc Euler (roll, pitch, yaw)
                    glm::dvec3 eulerAngles = glm::eulerAngles(q1);
                    glm::dmat4 matrixRotate(1.0); 

                    if (eulerAngles.z != 0.0) {
                        matrixRotate = glm::rotate(matrixRotate, eulerAngles.z, glm::dvec3(0.0, 0.0, 1.0));
                    }
                    if (eulerAngles.y != 0.0) {
                        matrixRotate = glm::rotate(matrixRotate, eulerAngles.y, glm::dvec3(0.0, 1.0, 0.0));
                    }
                    if (eulerAngles.x != 0.0) {
                        matrixRotate = glm::rotate(matrixRotate, eulerAngles.x, glm::dvec3(1.0, 0.0, 0.0));
                    }

                    double scaleZ = Convert::computeScaleZForLevel(root_tile_id.z);
                    double metersPerExtentUnit = dragon::CesiumHelper::getMetersPerExtentUnit(wgs84Rtc.value().y);
                    metersPerExtentUnit = 14;
                    glm::dmat4 scaleMatrix = glm::scale(glm::dmat4(1.0), glm::dvec3(1.0 * metersPerExtentUnit,
                        -1.0 * metersPerExtentUnit,
                        1.0 * scaleZ * metersPerExtentUnit));

                    glm::dmat4 translateBackMatrix = glm::translate(glm::dmat4(1.0), glm::dvec3(tile.localX,
                        tile.localY,
                        wgs84Rtc.value().z));


                    matrix_transform = translateBackMatrix * scaleMatrix * matrixRotate * CesiumGeometry::Transforms::Y_UP_TO_Z_UP * translateToOriginMatrix; 

                    threepp::Matrix4 tmat;
                    for (int col = 0; col < 4; ++col) {
                        for (int row = 0; row < 4; ++row) {
                            tmat.elements[col * 4 + row] = static_cast<float>(matrix_transform[col][row]);
                        }
                    }

                    tmp->traverseType<threepp::Mesh>([&](threepp::Mesh& child) {
                        child.geometry()->applyMatrix4(tmat);
                        child.geometry()->computeBoundingBox();
                        child.geometry()->computeBoundingSphere();
                        child.geometry()->computeVertexNormals();
                        });
                   /* tmp->position.set(tile.localX,
                        tile.localY,
                        wgs84Rtc.value().z);*/
                    tmp->matrixAutoUpdate = false;




                   // glm::dvec3 localUp = ellipsoid.geodeticSurfaceNormal(extension.rtcCenter);
                   // glm::dvec3 maplibreZUp = glm::dvec3(0, 0, 1);

                   // // Normalize local up vector
                   // glm::dvec3 worldUp = glm::normalize(ellipsoid.geodeticSurfaceNormal(extension.rtcCenter));
                   // glm::dvec3 targetUp = glm::dvec3(0, 0, 1);

                   // double dotProduct = glm::clamp(glm::dot(worldUp, targetUp), -1.0, 1.0);
                   // double angle = std::acos(dotProduct);

                   // glm::dquat q;
                   // if (angle > 0.001) { // Tránh góc quá nhỏ
                   //     glm::dvec3 axis = glm::normalize(glm::cross(worldUp, targetUp));
                   //     q = glm::angleAxis(angle, axis);
                   // }
                   // else {
                   //     q = glm::dquat(1, 0, 0, 0); // Identity quaternion
                   // }


                   // threepp::Quaternion tq(
                   // 	static_cast<float>(q.x),
                   // 	static_cast<float>(q.y),
                   // 	static_cast<float>(q.z),
                   // 	static_cast<float>(q.w)
                   // );
                   // threepp::Euler euler; 
                   // euler.setFromQuaternion(tq);
                   // double eulerX = threepp::math::radToDeg(euler.x); 
                   // double eulerY = threepp::math::radToDeg(euler.y);
                   // double eulerZ = threepp::math::radToDeg(euler.z);
         
                   // glm::dmat4 matrix_rotate = dragon::CesiumHelper::createMatrixRotateAroundPivot(dcenter,
                   //     eulerAngles.x,
                   //     eulerAngles.y,
                   //     eulerAngles.z);
                   // transformed_center1 = matrix_rotate * glm::dvec4(dcenter, 1.0);
                   // //dcenter = glm::dvec3(transformed_center1); 

                   // double scaleZ = Convert::computeScaleZForLevel(root_tile_id.z);
                   // double metersPerExtentUnit = dragon::CesiumHelper::getMetersPerExtentUnit(wgs84Rtc.value().y);
                   // glm::dmat4 matrix_scale = dragon::CesiumHelper::createMatrixScaleAroundPivot(dcenter,
                   //     1.0 * metersPerExtentUnit,
                   //    1.0 * metersPerExtentUnit,
                   //     1.0 * scaleZ * metersPerExtentUnit);
                   // //transformed_center1 = matrix_scale * glm::dvec4(dcenter, 1.0);
                   // ///dcenter = glm::dvec3(transformed_center1);
                   //
                   ///* glm::dmat4 matrix_translate = dragon::CesiumHelper::createMatrixTranslateAroundPivot(glm::dvec3(0,0,0),
                   //     tile.localX,
                   //     tile.localY,
                   //     wgs84Rtc.value().z);*/

                   // glm::dmat4 matrix_translate = glm::translate(glm::dmat4(1.0), glm::dvec3(tile.localX,
                   //     tile.localY,
                   //     wgs84Rtc.value().z));

                   // glm::dmat4 result_matrix = matrix_translate * matrix_scale * matrix_rotate * CesiumGeometry::Transforms::Y_UP_TO_Z_UP;
                   // threepp::Matrix4 tmat; 
                   // for (int col = 0; col < 4; ++col) {
                   //     for (int row = 0; row < 4; ++row) {
                   //         tmat.elements[col * 4 + row] = static_cast<float>(result_matrix[col][row]);
                   //     }
                   // }
                   // tmp->traverseType<threepp::Mesh>([&](threepp::Mesh& child) {
                   //   child.geometry()->applyMatrix4(tmat);
                   //   child.geometry()->computeBoundingBox();
                   //   child.geometry()->computeBoundingSphere();
                   //   child.geometry()->computeVertexNormals();
                   //  });
                   // tmp->matrixAutoUpdate = false;
                }
                //scene->add(orientedBoundingBox);
                scene->add(tmp); 
                t_count++; 
            }
            delete loadThreadResult;
        }
        return new AllocationResult{ totalAllocation };
    }

    void MaplibrePrepareRendererResource::free(
        Cesium3DTilesSelection::Tile& tile,
        void* pLoadThreadResult,
        void* pMainThreadResult) noexcept {
        if (pMainThreadResult) {
            AllocationResult* mainThreadResult =
                reinterpret_cast<AllocationResult*>(pMainThreadResult);
            delete mainThreadResult;
        }

        if (pLoadThreadResult) {
            AllocationResult* loadThreadResult =
                reinterpret_cast<AllocationResult*>(pLoadThreadResult);
            delete loadThreadResult;
        }
    }
}