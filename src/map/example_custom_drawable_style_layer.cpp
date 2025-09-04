#include "example_custom_drawable_style_layer.hpp"
#include "tools/Gizmo.hpp"
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include <memory>
#include <cmath>
#include <filesystem>
#include "threepp/threepp.hpp"
#include "threepp/core/Raycaster.hpp"
#include "threepp/helpers/PlaneHelper.hpp"
#include "threepp/loaders/AssimpLoader.hpp"
#include <mbgl/helper/MecatorHelper.hpp>
#include <spdlog/spdlog.h>
#include "core/utils/ThreeHelper.hpp"
#include "core/utils/StringHelper.hpp"
#include "core/utils/CesiumHelper.hpp"
#include <Cesium3DTilesSelection/ViewState.h>
#include <Cesium3DTilesSelection/Tileset.h>

static std::shared_ptr<threepp::PlaneHelper> makePlaneFromAxis(const threepp::Vector3& normal, const threepp::Vector3& gizmoPos)
{

    threepp::Plane plane; 
    plane.setFromNormalAndCoplanarPoint(normal, gizmoPos);
    auto helper = threepp::PlaneHelper::create(plane,50.0, threepp::Color::red);
    helper->updateMatrixWorld(true);
    return helper; 
}

ThreeDCustomDrawableStyleLayerHost::ThreeDCustomDrawableStyleLayerHost()
{
    m_RayCaster = std::make_unique<threepp::Raycaster>();
    m_RayCaster->params.lineThreshold = 0.1f; 
}

ThreeDCustomDrawableStyleLayerHost::~ThreeDCustomDrawableStyleLayerHost() {}

void ThreeDCustomDrawableStyleLayerHost::initialize() {}

void ThreeDCustomDrawableStyleLayerHost::deinitialize() {}

void ThreeDCustomDrawableStyleLayerHost::update(Interface& interface) {
    // if we have built our drawable(s) already, either update or skip
    if (interface.getDrawableCount() == 0)
    {
        interface.addCustomDrawableWithTile({ 16, 53558, 28597 });
        m_LayerGroup = interface.getLayerGroupBase(); 
        return;
    }

    if (!fnc_queue.empty())
    {
        std::function<void()> fnc = fnc_queue.front(); 
        if (fnc)
            fnc(); 
        fnc_queue.pop(); 
    }
}

void ThreeDCustomDrawableStyleLayerHost::openEditMode(bool open)
{
    mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(m_LayerGroup.get());
    tileLayerGroup->visitDrawables([&](const mbgl::gfx::Drawable& drawable) {
        if (drawable.getDrawType() == mbgl::gfx::Drawable::DrawableType::DrawableCustom)
        {
            const mbgl::gfx::Drawable* ptrDrawable = &drawable;
            const mbgl::gl::DrawableCustom* ptrDrawableCustom = static_cast<const mbgl::gl::DrawableCustom*>(ptrDrawable);
            if (ptrDrawableCustom)
            {
                auto impl = ptrDrawableCustom->getImpl();
                if (impl->scene)
                {
                    auto bim_model = impl->scene->getObjectByName("model"); 
                    auto gizmo = impl->scene->getObjectByName("gizmo"); 
                    gizmo->visible = open;
                }
            }
        }}); 
}


void ThreeDCustomDrawableStyleLayerHost::query(threepp::Vector2 nor_pos)
{
    m_nor_pos = nor_pos;
    if (!m_LayerGroup)
        return; 
    mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(m_LayerGroup.get());
    tileLayerGroup->visitDrawables([&](const mbgl::gfx::Drawable& drawable) {
        if (drawable.getDrawType() == mbgl::gfx::Drawable::DrawableType::DrawableCustom)
        {
            const mbgl::gfx::Drawable* ptrDrawable = &drawable;
            const mbgl::gl::DrawableCustom* ptrDrawableCustom = static_cast<const mbgl::gl::DrawableCustom*>(ptrDrawable);
            if (ptrDrawableCustom)
            {
                auto impl = ptrDrawableCustom->getImpl();
                if (!impl) return; 
                auto scene = impl->scene.get(); 
                auto camera = impl->camera.get(); 
                if (scene && camera && m_RayCaster)
                {
                    m_RayCaster->setFromCamera(nor_pos, *camera); 
                    auto gizmo = scene->getObjectByName("gizmo"); 
                    if (!gizmo || !gizmo->visible)return;
                    threepp::Vector3 cam_dir;
                    const auto& e = camera->matrixWorld->elements;
                    cam_dir.set(e[8], e[9], e[10]).normalize();
                    cam_dir.negate();
                    isDrag = true;
                    m_Gizmo->startDrag(m_RayCaster.get(), cam_dir);
                }
            }
        }});
}

void ThreeDCustomDrawableStyleLayerHost::mouseRelease(threepp::Vector2 nor_pos)
{
    isDrag = false;
    m_Gizmo->endDrag(); 
}

void ThreeDCustomDrawableStyleLayerHost::mouseMove(threepp::Vector2 nor_pos)
{
    if (isDrag)
    {
        if (!m_LayerGroup)
            return;
        mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(m_LayerGroup.get());
        tileLayerGroup->visitDrawables([&](const mbgl::gfx::Drawable& drawable) {
            if (drawable.getDrawType() == mbgl::gfx::Drawable::DrawableType::DrawableCustom)
            {
                const mbgl::gfx::Drawable* ptrDrawable = &drawable;
                const mbgl::gl::DrawableCustom* ptrDrawableCustom = static_cast<const mbgl::gl::DrawableCustom*>(ptrDrawable);
                if (ptrDrawableCustom)
                {
                    auto impl = ptrDrawableCustom->getImpl();
                    if (!impl) return;
                    auto scene = impl->scene.get();
                    auto camera = impl->camera.get();
                    if (scene && camera && m_RayCaster)
                    {
                        threepp::Vector3 cam_dir;
                        const auto& e = camera->matrixWorld->elements;
                        cam_dir.set(e[8], e[9], e[10]).normalize();
                        cam_dir.negate();
                        m_RayCaster->setFromCamera(nor_pos, *camera);
                        if (m_Gizmo->dragging)
                        {
                            m_Gizmo->updateDrag(m_RayCaster.get(), cam_dir);
                        }
                    }
                }
            }});
    }
}

void ThreeDCustomDrawableStyleLayerHost::addBim(std::shared_ptr<threepp::Object3D> bim_model)
{
    auto lambda = [&,bim_model]() { mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(m_LayerGroup.get());
    tileLayerGroup->visitDrawables([&](const mbgl::gfx::Drawable& drawable) {
        if (drawable.getDrawType() == mbgl::gfx::Drawable::DrawableType::DrawableCustom)
        {
            const mbgl::gfx::Drawable* ptrDrawable = &drawable;
            const mbgl::gl::DrawableCustom* ptrDrawableCustom = static_cast<const mbgl::gl::DrawableCustom*>(ptrDrawable);
            if (ptrDrawableCustom)
            {
                auto impl = ptrDrawableCustom->getImpl();
                if (impl->scene)
                {
                    auto model = impl->scene->getObjectByName("model");
                    float scale = static_cast<float>(mbgl::gl::MecatorHelper::computeScaleZForLevel(16));
                    if (!model)
                    {
                        auto addLight_lambda = [&](threepp::Scene& scene) {
                            auto a_light = threepp::AmbientLight::create(0xffffee, std::optional(0.4f));
                            scene.add(a_light);
                            std::shared_ptr<threepp::Light> directionalLight1 = threepp::DirectionalLight::create(0xffeeff, std::optional(0.7f));
                            directionalLight1->position.set(1, 1, 1);
                            scene.add(directionalLight1);
                            std::shared_ptr<threepp::Light> directionalLight2 = threepp::DirectionalLight::create(0xffeeff, std::optional(0.7f));
                            directionalLight2->position.set(-1, 0.5, -1);
                            scene.add(directionalLight2);
                        }; 

                        auto addGizmo_lambda = [&](threepp::Scene& scene){
                            m_Gizmo = std::make_shared<dragon::Gizmo>();
                            std::shared_ptr<threepp::Group> gizmo_group = m_Gizmo->create();
                            gizmo_group->name = "gizmo";
                            threepp::Matrix4 scale_mat; 
                            scale_mat.makeScale(1.0, 1.0, scale * 1.0f);
                          /*  std::function<void(threepp::Object3D*,threepp::Matrix4&)> dfs =
                                [&](threepp::Object3D* obj, threepp::Matrix4& root_mat) {
                                if (auto mesh = obj->as<threepp::Mesh>()) {
                                    if (mesh)
                                    {
                                        if (mesh && mesh->geometry()) {
                                            threepp::Matrix4 mat; 
                                            mat.multiplyMatrices(*mesh->matrix, root_mat); 
                                            mesh->geometry()->applyMatrix4(mat);
                                            mesh->geometry()->computeBoundingSphere();
                                            mesh->geometry()->computeBoundingBox();
                                            return; 
                                        }
                                    }
                                }
                                else if (auto group = obj->as<threepp::Group>())
                                {
                                    root_mat.multiply(*obj->matrixWorld);
                                }

                                for (auto& child : obj->children) {
                                    dfs(child, root_mat);
                                }
                            }; */
                            gizmo_group->scale.set(1.0, 1.0, scale);
                            gizmo_group->visible = false;
                            gizmo_group->matrixAutoUpdate = true;
                            gizmo_group->updateMatrixWorld(true); 
                            scene.add(gizmo_group);
                        }; 
                        addLight_lambda(*impl->scene);
                        addGizmo_lambda(*impl->scene); 
                    }
                    else
                    {
                        impl->scene->remove(*model); 
                    }

                    auto add_model_lambda = [&,bim_model](threepp::Scene& scene) {
                        auto mesh = bim_model->as<threepp::Mesh>(); 
                        // 1. Scale
                        auto matrix_scale = dragon::ThreeHelper::createMatrixScaleAroundPivot(
                            threepp::Vector3(0, 0, 0),
                            1.0 * 10, -scale * 10, 1.0 * 10
                        );
                        // 2. Rotate
                        auto matrix_rotate = dragon::ThreeHelper::createMatrixRotateAroundPivot(
                            threepp::Vector3(0, 0, 0),
                            threepp::math::degToRad(-90), 0.0, 0.0
                        );
                        // 3. Translate
                        auto matrix_translate = dragon::ThreeHelper::createMatrixTranslateAroundPivot(
                            threepp::Vector3(0.0, 0.0, 0.0),
                            0.0, 0.0, 0
                        );
                        threepp::Matrix4 combined;
                        combined.multiplyMatrices(matrix_rotate, matrix_scale); // T * R
                        mesh->geometry()->applyMatrix4(combined);
                        mesh->geometry()->computeBoundingBox(); 
                        mesh->geometry()->computeBoundingSphere(); 
                        mesh->geometry()->computeVertexNormals(); 
                        mesh->applyMatrix4(matrix_translate); 
                        mesh->name = "model";
                        scene.add(bim_model);
                        m_Gizmo->setTarget(bim_model.get()); 
                    }; 
                    add_model_lambda(*impl->scene); 

                   /* const std::string pathGLB = "C:\\Users\\ntlon\\Downloads\\a.b3dm.glb";
                    std::vector<std::byte> glbFile = dragon::StringHelper::readFile(pathGLB);
                    auto test_model = dragon::CesiumHelper::createGLB(glbFile);
                    threepp::Matrix4 matrix_scale; 
                    matrix_scale.identity(); 
                    threepp::Box3 box; 
                    box.setFromObject(*test_model); 
                    matrix_scale = dragon::ThreeHelper::createMatrixScaleAroundPivot(
                        box.getCenter(),
                        1.0, 1.0, scale
                    );
                    threepp::Matrix4 rotate; 
                    rotate.makeRotationY(threepp::math::degToRad(180));
                    test_model->applyMatrix4(rotate); 
                    test_model->applyMatrix4(matrix_scale); 
                    impl->scene->add(test_model);*/
                    //m_Gizmo->setTarget(*impl->scene);
                }
            }
        }
        });
        }; 
    fnc_queue.push(std::move(lambda));
}

dragon::Gizmo* ThreeDCustomDrawableStyleLayerHost::getGizmo() const
{
    return m_Gizmo.get(); 
}

Plane ThreeDCustomDrawableStyleLayerHost::chooseBestPlane(const threepp::Vector3& gizmoOrigin, const threepp::Vector3& selectedAxis, const threepp::Vector3& cameraDir)
{
    threepp::Vector3 axisX{ 1,0,0 }, axisY{ 0,1,0 }, axisZ{ 0,0,1 };
    std::vector<threepp::Vector3> candidates;

    if (selectedAxis.equals(axisX)) {
        candidates.push_back(axisY);
        candidates.push_back(axisZ);
    }
    else if (selectedAxis.equals(axisY)) {
        candidates.push_back(axisX);
        candidates.push_back(axisZ);
    }
    else if (selectedAxis.equals(axisZ)) {
        candidates.push_back(axisX);
        candidates.push_back(axisY);
    }

    Plane bestPlane;
    float bestDot = -1.0f;

    for (auto& otherAxis : candidates) {
        threepp::Vector3 normal = selectedAxis.clone().cross(otherAxis).normalize();
        float d = std::fabs(normal.dot(cameraDir));
        if (d > bestDot) {
            bestDot = d;
            bestPlane = { gizmoOrigin, normal };
        }
    }

    return bestPlane;
}

mbgl::Point<double> ThreeDCustomDrawableStyleLayerHost::project(const mbgl::LatLng& c, const mbgl::TransformState& s) {
    mbgl::LatLng unwrappedLatLng = c.wrapped();
    unwrappedLatLng.unwrapForShortestPath(s.getLatLng(mbgl::LatLng::Wrapped));
    return mbgl::Projection::project(unwrappedLatLng, s.getScale());
}
