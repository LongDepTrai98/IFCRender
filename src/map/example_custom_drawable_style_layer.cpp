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
#include <threepp/threepp.hpp>
#include <threepp/core/Raycaster.hpp>
#include <mbgl/helper/MecatorHelper.hpp>
#include <spdlog/spdlog.h>
#include "core/utils/ThreeHelper.hpp"

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
        interface.addCustomDrawableWithTile({ 15, 26093, 15394 });
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
                    if (bim_model)
                        bim_model->visible = !open;
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
                    const auto intersects = m_RayCaster->intersectObjects(gizmo->children,true);
                    if (intersects.size() != 0) {
                        spdlog::info("Intersect"); 
                    };
                }
            }
        }});
}

void ThreeDCustomDrawableStyleLayerHost::addLight(threepp::Scene& scene)
{
    auto a_light = threepp::AmbientLight::create(0xffffee, std::optional(0.4f));
    scene.add(a_light);
    std::shared_ptr<threepp::Light> directionalLight1 = threepp::DirectionalLight::create(0xffeeff, std::optional(0.7f));
    directionalLight1->position.set(1, 1, 1);
    scene.add(directionalLight1);
    std::shared_ptr<threepp::Light> directionalLight2 = threepp::DirectionalLight::create(0xffeeff, std::optional(0.7f));
    directionalLight2->position.set(-1, 0.5, -1);
    scene.add(directionalLight2);

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
                    float scale_z = static_cast<float>(mbgl::gl::MecatorHelper::computeScaleZForLevel(15));
                    auto model = impl->scene->getObjectByName("model");
                    if (!model)
                    {
                        addLight(*impl->scene);
                        m_Gizmo = std::make_shared<dragon::Gizmo>(); 
                        std::shared_ptr<threepp::Group> arrow_group = m_Gizmo->create();
                        arrow_group->name = "gizmo";
                        arrow_group->scale.set(1.0, 1.0, scale_z);
                        arrow_group->visible = false; 
                        arrow_group->matrixAutoUpdate = true; 
                        arrow_group->updateMatrixWorld(true); 
                        impl->scene->add(arrow_group);
                        spdlog::info("Create gizmo arrow");
                    }
                    else
                    {
                        impl->scene->remove(*model); 
                    }
                    auto root_matrix = bim_model->matrix;
                    auto matrix_scale = dragon::ThreeHelper::createMatrixScaleAroundPivot(threepp::Vector3(0, 0, 0), 1.0 * 10.0, -scale_z * 10.0, 1.0 * 10.0);
                    bim_model->as<threepp::Mesh>()->applyMatrix4(matrix_scale);
                    auto matrix_rotate = dragon::ThreeHelper::createMatrixRotateAroundPivot(threepp::Vector3(0, 0, 0), threepp::math::degToRad(-90), 0.0, 0.0);
                    bim_model->as<threepp::Mesh>()->applyMatrix4(matrix_rotate);
                    auto matrix_translate = dragon::ThreeHelper::createMatrixTranslateAroundPivot(threepp::Vector3(0, 0, 0), 4096.0, 4096.0, 0.0);
                    bim_model->as<threepp::Mesh>()->applyMatrix4(matrix_translate);
                    bim_model->matrixAutoUpdate = false;
                    bim_model->updateMatrixWorld(true); 
                    bim_model->name = "model"; 
                    impl->scene->add(bim_model);
                    m_Gizmo->setTarget(bim_model.get()); 
                    spdlog::info("show model bim");
                }
            }
        }
        });
        }; 
    fnc_queue.push(std::move(lambda));
}

mbgl::Point<double> ThreeDCustomDrawableStyleLayerHost::project(const mbgl::LatLng& c, const mbgl::TransformState& s) {
    mbgl::LatLng unwrappedLatLng = c.wrapped();
    unwrappedLatLng.unwrapForShortestPath(s.getLatLng(mbgl::LatLng::Wrapped));
    return mbgl::Projection::project(unwrappedLatLng, s.getScale());
}
