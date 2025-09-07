#include <Cesium3DTilesSelection/TileContent.h>
#include <CesiumGeospatial/GlobeTransforms.h>
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

            //if (!scene->getObjectByName(tile_str_id))
            {
                Cesium3DTilesSelection::BoundingVolume bouding_tile_voulume = tile.getBoundingVolume();
                std::shared_ptr<threepp::Mesh> orientedBoundingBox = dragon::CesiumHelper::createOrientedBoundingBox(bouding_tile_voulume, root_tile_id);
                
                orientedBoundingBox->name = tile_str_id;
                Cesium3DTilesSelection::TileRenderContent* renderContent = tile.getContent().getRenderContent(); 
                CesiumGltf::Model model = renderContent->getModel(); 
                dragon::CesiumHelper::B3DMExtensions extension;
                auto tmp = dragon::CesiumHelper::createMesh(model, extension);
                threepp::Box3 box;
                box.setFromObject(*tmp);
                if (extension.hasRTC)
                {
                    const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
                    std::optional<glm::dvec3> wgs84Rtc = dragon::CesiumHelper::ecefToWgs84(extension.rtcCenter);
                    auto tile = Convert::wgs84ToLocalInTile(wgs84Rtc.value().x, wgs84Rtc.value().y, root_tile_id.x, root_tile_id.y); 

                    glm::dvec3 worldUp = ellipsoid.geodeticSurfaceNormal(extension.rtcCenter);
                    double angle = std::acos(glm::dot(worldUp, glm::dvec3(0, 0, 1)));
                    //truc quay 
                    glm::dvec3 axis = glm::normalize(glm::cross(worldUp, glm::dvec3(0, 0, 1))); 
                    glm::dquat q = glm::angleAxis(angle, axis);
                    threepp::Quaternion tq(
                    	static_cast<float>(q.x),
                    	static_cast<float>(q.y),
                    	static_cast<float>(q.z),
                    	static_cast<float>(q.w)
                    );
                    threepp::Euler euler; 
                    euler.setFromQuaternion(tq);
                    double eulerX = threepp::math::radToDeg(euler.x); 
                    double eulerY = threepp::math::radToDeg(euler.y);
                    double eulerZ = threepp::math::radToDeg(euler.z);
                    threepp::Matrix4 matrix_rotate = dragon::ThreeHelper::createMatrixRotateAroundPivot(box.getCenter(),
                    	euler.x,
                    	euler.y,
                    	 2 * euler.z); 
                    tmp->traverseType<threepp::Mesh>([&](threepp::Mesh& child) {
                        child.geometry()->applyMatrix4(matrix_rotate); 
                        child.geometry()->computeBoundingBox(); 
                        child.geometry()->computeBoundingSphere(); 
                        child.geometry()->computeVertexNormals(); 
                        });
                    box.setFromObject(*tmp);


                    double scaleZ = Convert::computeScaleZForLevel(root_tile_id.z);
                    double metersPerExtentUnit = dragon::CesiumHelper::getMetersPerExtentUnit(wgs84Rtc.value().y);
                    threepp::Matrix4 scale_matrix = dragon::ThreeHelper::createMatrixScaleAroundPivot(box.getCenter(),
                        1.0 * metersPerExtentUnit,
                        -1.0 * metersPerExtentUnit,
                        -1.0 * scaleZ * metersPerExtentUnit);
                    tmp->traverseType<threepp::Mesh>([&](threepp::Mesh& child) {
                        child.geometry()->applyMatrix4(scale_matrix);
                        child.geometry()->computeBoundingBox();
                        child.geometry()->computeBoundingSphere();
                        child.geometry()->computeVertexNormals();
                        });
                    box.setFromObject(*tmp);
                    threepp::Matrix4 translate_matrix = dragon::ThreeHelper::createMatrixTranslateAroundPivot(box.getCenter(),
                        tile.localX,
                        tile.localY,
                        wgs84Rtc.value().z);
                    tmp->applyMatrix4(translate_matrix); 
                    tmp->matrixAutoUpdate = false;
                    tmp->updateMatrixWorld(true);

                }
                scene->add(orientedBoundingBox);
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