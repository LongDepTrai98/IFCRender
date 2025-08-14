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
                    m_Obj->scale.set(1.0 * 10.0,-scale_z * 10.0,1.0 * 10.0);
                    m_Obj->rotation.x = threepp::math::degToRad(-90);
                    m_Obj->matrixAutoUpdate = true;
                    impl->scene->clear(); 
                    impl->scene->add(m_Obj);
                    addLight(*impl->scene); 
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

mbgl::Point<double> ThreeDCustomDrawableStyleLayerHost::project(const mbgl::LatLng& c, const mbgl::TransformState& s) {
    mbgl::LatLng unwrappedLatLng = c.wrapped();
    unwrappedLatLng.unwrapForShortestPath(s.getLatLng(mbgl::LatLng::Wrapped));
    return mbgl::Projection::project(unwrappedLatLng, s.getScale());
}
