#pragma once
#ifdef interface __STRUCT__
#undef interface __STRUCT__
#endif // interface __STRUCT__
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include "threepp/core/Raycaster.hpp"
#include "threepp/math/infinity.hpp"
#include <queue>
#include <functional>
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
    ThreeDCustomDrawableStyleLayerHost();
    ~ThreeDCustomDrawableStyleLayerHost();
    void initialize() override;
    void deinitialize() override;
    void update(Interface& interface) override;
public: 
    void addGizmo(); 
    void createGizmo(Interface& interface);
    void query(threepp::Vector2 nor_pos);
    void addBim(std::shared_ptr<threepp::Object3D> bim_model); 
private: 
    void addLight(threepp::Scene& main_scene); 
protected:
    std::queue<std::function<void()>> fnc_queue{};  
    bool m_bAddGizmo{ false };
    bool m_bIsClick{ false }; 
    std::shared_ptr<dragon::Gizmo> m_Gizmo{ nullptr };
    std::shared_ptr<mbgl::LayerGroupBase> m_LayerGroup{ nullptr }; 
    threepp::Vector2 m_nor_pos{ -threepp::Infinity<float>, -threepp::Infinity<float> };
    static mbgl::Point<double> project(const mbgl::LatLng& c, const mbgl::TransformState& s);
protected:
};
