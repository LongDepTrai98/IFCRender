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
#include <mbgl/helper/MecatorHelper.hpp>
#include <spdlog/spdlog.h>
#include "core/utils/ThreeHelper.hpp"

ThreeDCustomDrawableStyleLayerHost::ThreeDCustomDrawableStyleLayerHost()
{
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

    if (m_bAddGizmo)
    {
        createGizmo(interface); 
    }
    if (m_bIsClick)
    {
        interface.updatePosMouseNor(m_nor_pos.x, m_nor_pos.y);
        m_bIsClick = false; 
    }
}

void ThreeDCustomDrawableStyleLayerHost::addGizmo()
{
    m_bAddGizmo = true; 
}

void ThreeDCustomDrawableStyleLayerHost::query(threepp::Vector2 nor_pos)
{
    m_nor_pos = nor_pos; 
    m_bIsClick = true; 

    if (!m_LayerGroup)
        return; 


    auto test = m_LayerGroup->getDrawableCount(); 
    mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(m_LayerGroup.get());
    tileLayerGroup->visitDrawables([&](const mbgl::gfx::Drawable& drawable) {
        int a = 3; 
        }); 
   /* auto& layerGroup = interface.getLayerGroupBase();
    mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(layerGroup.get());
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
                    if (!model)
                        return;
                    model->visible = false;
                    float scale_z = static_cast<float>(mbgl::gl::MecatorHelper::computeScaleZForLevel(15));
                    m_Gizmo = std::make_shared<dragon::Gizmo>();
                    std::shared_ptr<threepp::Group> arrow_group = m_Gizmo->create(model);
                    arrow_group->name = "gizmo";
                    arrow_group->scale.set(1.0, 1.0, scale_z);
                    impl->scene->add(arrow_group);
                    spdlog::info("Create gizmo arrow");
                }
            }
        }
        });*/
}

void ThreeDCustomDrawableStyleLayerHost::createGizmo(Interface& interface)
{
    auto& layerGroup = interface.getLayerGroupBase();
    mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(layerGroup.get());
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
                    if (!model)
                        return;
                    model->visible = false; 
                    float scale_z = static_cast<float>(mbgl::gl::MecatorHelper::computeScaleZForLevel(15));
                    m_Gizmo = std::make_shared<dragon::Gizmo>(); 
                    std::shared_ptr<threepp::Group> arrow_group = m_Gizmo->create(model); 
                    arrow_group->name = "gizmo"; 
                    arrow_group->scale.set(1.0, 1.0, scale_z); 
                    impl->scene->add(arrow_group);
                    spdlog::info("Create gizmo arrow"); 
                }
            }
        }
        });
    m_bAddGizmo = false; 
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

                    auto model = impl->scene->getObjectByName("model");
                    if (!model)
                    {
                        addLight(*impl->scene);
                    }
                    else
                    {
                        impl->scene->remove(*model); 
                    }
                    auto root_matrix = bim_model->matrix;
                    float scale_z = static_cast<float>(mbgl::gl::MecatorHelper::computeScaleZForLevel(15));
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
