#pragma once
#ifdef interface __STRUCT__
#undef interface __STRUCT__
#endif // interface __STRUCT__
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include "threepp/core/Raycaster.hpp"
#include "threepp/math/infinity.hpp"
namespace threepp
{
    class Object3D;
    class Scene; 
}

namespace dragon
{
    class Gizmo; 
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
    void addGizmo(); 
    void createGizmo(Interface& interface);
    void testRay(threepp::Vector2 nor_pos);
private: 
    void addLight(threepp::Scene& main_scene); 
    void addBim(Interface& interface);
protected:
    bool isAdded{ false }; 
    bool m_bAddGizmo{ false };
    bool m_bIsClick{ false }; 
    std::shared_ptr<threepp::Object3D> m_Obj{ nullptr };
    std::shared_ptr<dragon::Gizmo> m_Gizmo{ nullptr };
    threepp::Vector2 m_nor_pos{ -threepp::Infinity<float>, -threepp::Infinity<float> };
    static mbgl::Point<double> project(const mbgl::LatLng& c, const mbgl::TransformState& s);
protected:
};
