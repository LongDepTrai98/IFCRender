#pragma once
#ifdef interface __STRUCT__
#undef interface __STRUCT__
#endif // interface __STRUCT__

#include <mbgl/style/layers/custom_drawable_layer.hpp>

class ExampleCustomDrawableStyleLayerHost : public mbgl::style::CustomDrawableLayerHost {
public:
    using TriangleIndexVector = mbgl::gfx::IndexVector<mbgl::gfx::Triangles>;
    ExampleCustomDrawableStyleLayerHost(const std::string& assetsPath);
    ~ExampleCustomDrawableStyleLayerHost();

    void initialize() override;
    void deinitialize() override;

    void update(Interface& interface) override;

protected:
    static mbgl::Point<double> project(const mbgl::LatLng& c, const mbgl::TransformState& s);
protected:
    const std::string assetsPath;
};
