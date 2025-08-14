#pragma once
#ifdef interface __STRUCT__
#undef interface __STRUCT__
#endif // interface __STRUCT__
#include <mbgl/style/layers/custom_drawable_layer.hpp>

namespace threepp
{
    class Object3D;
    class Scene; 
}

class ThreeDCustomDrawableStyleLayerHost : public mbgl::style::CustomDrawableLayerHost {
public:
    using TriangleIndexVector = mbgl::gfx::IndexVector<mbgl::gfx::Triangles>;
    ThreeDCustomDrawableStyleLayerHost(std::shared_ptr<threepp::Object3D> obj);
    ~ThreeDCustomDrawableStyleLayerHost();
    void initialize() override;
    void deinitialize() override;
    void update(Interface& interface) override;
public: 
    void addBim(Interface& interface);
private: 
    void addLight(threepp::Scene& main_scene); 
protected:
    bool isAdded{ false }; 
    std::shared_ptr<threepp::Object3D> m_Obj{ nullptr };
    static mbgl::Point<double> project(const mbgl::LatLng& c, const mbgl::TransformState& s);
protected:
};
