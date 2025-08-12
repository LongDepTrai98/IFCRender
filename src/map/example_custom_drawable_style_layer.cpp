#include "example_custom_drawable_style_layer.hpp"

#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/util/logging.hpp>

#include <memory>
#include <cmath>
#include <filesystem>


ExampleCustomDrawableStyleLayerHost::ExampleCustomDrawableStyleLayerHost(const std::string& assetsPath_)
    : assetsPath(assetsPath_) {}

ExampleCustomDrawableStyleLayerHost::~ExampleCustomDrawableStyleLayerHost() {}

void ExampleCustomDrawableStyleLayerHost::initialize() {}

void ExampleCustomDrawableStyleLayerHost::deinitialize() {}

void ExampleCustomDrawableStyleLayerHost::update(Interface& interface) {
    // if we have built our drawable(s) already, either update or skip
    if (interface.getDrawableCount() == 0) {
        interface.addCustomDrawable(); 
        return;
    }
}

mbgl::Point<double> ExampleCustomDrawableStyleLayerHost::project(const mbgl::LatLng& c, const mbgl::TransformState& s) {
    mbgl::LatLng unwrappedLatLng = c.wrapped();
    unwrappedLatLng.unwrapForShortestPath(s.getLatLng(mbgl::LatLng::Wrapped));
    return mbgl::Projection::project(unwrappedLatLng, s.getScale());
}
