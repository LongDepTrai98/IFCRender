#include "example_custom_drawable_style_layer.hpp"
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

ThreeDCustomDrawableStyleLayerHost::ThreeDCustomDrawableStyleLayerHost(std::shared_ptr<threepp::Object3D> obj) : m_Obj(obj)
{
}

ThreeDCustomDrawableStyleLayerHost::~ThreeDCustomDrawableStyleLayerHost() {}

void ThreeDCustomDrawableStyleLayerHost::initialize() {}

void ThreeDCustomDrawableStyleLayerHost::deinitialize() {}

void ThreeDCustomDrawableStyleLayerHost::addBim(Interface& interface)
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
                    float scale_z = static_cast<float>(mbgl::gl::MecatorHelper::computeScaleZForLevel(15));
                  /*  m_Obj->rotation.x = threepp::math::degToRad(-90);
                    m_Obj->updateMatrix(); 
                    m_Obj->rotation.z = threepp::math::degToRad(-90);*/
                    m_Obj->scale.set(1.0 * 10.0,-scale_z * 10.0,1.0 * 10.0);
                    m_Obj->rotation.x = threepp::math::degToRad(-90);
                    m_Obj->matrixAutoUpdate = true;
                    impl->scene->add(m_Obj);
                    /*const auto sphereGeometry = threepp::SphereGeometry::create(300);
                    const auto sphereMaterial = threepp::MeshLambertMaterial::create();
                    sphereMaterial->color = threepp::Color::darkgray;
                    sphereMaterial->side = threepp::Side::Double;
                    sphereMaterial->wireframe = false;
                    float scale_z = static_cast<float>(MecatorHelper::computeScaleZForLevel(16));
                    auto sphere = threepp::Mesh::create(sphereGeometry, sphereMaterial);
                    sphere->scale.set(1.0, 1.0, scale_z);
                    auto meter = MecatorHelper::pixelToMeter(scale_z, 150);
                    sphere->position.set(4096.0,4096.0,static_cast<float>(meter));
                    scene->add(sphere);
                    const auto boxGeometry = threepp::BoxGeometry::create(200,200,200);
                    const auto boxMaterial = threepp::MeshLambertMaterial::create();
                    auto zoom = MecatorHelper::solveZoomLevelFor1MeterPerPixel();
                    boxMaterial->color.setRGB(1, 0, 0);
                    boxMaterial->side = threepp::Side::Double;
                    auto box = threepp::Mesh::create(boxGeometry, boxMaterial);
                    box->scale.set(1.0, 1.0, scale_z);
                    box->position.set(0.0, 0.0, static_cast<float>(MecatorHelper::pixelToMeter(scale_z,100)));
                    scene->add(box); */
                    isAdded = true; 
                }
            }
        }
    }); 
    /*visit*/
}

void ThreeDCustomDrawableStyleLayerHost::update(Interface& interface) {
    // if we have built our drawable(s) already, either update or skip
    if (interface.getDrawableCount() == 0) {
        interface.addCustomDrawableWithTile({ 15, 26093, 15394 });
        return;
    }
    if (!isAdded)
    {
        addBim(interface);
    }


}

mbgl::Point<double> ThreeDCustomDrawableStyleLayerHost::project(const mbgl::LatLng& c, const mbgl::TransformState& s) {
    mbgl::LatLng unwrappedLatLng = c.wrapped();
    unwrappedLatLng.unwrapForShortestPath(s.getLatLng(mbgl::LatLng::Wrapped));
    return mbgl::Projection::project(unwrappedLatLng, s.getScale());
}
