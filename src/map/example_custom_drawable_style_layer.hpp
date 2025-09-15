#pragma once
#ifdef interface __STRUCT__
#undef interface __STRUCT__
#endif // interface __STRUCT__
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/tile/tile_id.hpp>
#include "threepp/core/Raycaster.hpp"
#include "threepp/math/infinity.hpp"
#include "threepp/math/Vector3.hpp"
#include "threepp/math/Matrix4.hpp"
#include <queue>
#include <functional>
namespace threepp
{
    class Object3D;
    class Scene; 
    class Raycaster; 
}

namespace dragon
{
    class Gizmo; 
}

struct Plane {
    threepp::Vector3 point;  
    threepp::Vector3 normal; 
};


class ThreeDCustomDrawableStyleLayerHost : public mbgl::style::CustomDrawableLayerHost {
public:
    using TriangleIndexVector = mbgl::gfx::IndexVector<mbgl::gfx::Triangles>;
    ThreeDCustomDrawableStyleLayerHost(std::string filePath);
    ~ThreeDCustomDrawableStyleLayerHost();
    void initialize() override;
    void deinitialize() override;
    void update(Interface& interface) override;
    void printMatrix(); 
public: 
    void openEditMode(bool open);
    void query(threepp::Vector2 nor_pos);
    void mouseRelease(threepp::Vector2 nor_pos);
    void mouseMove(threepp::Vector2 nor_pos);
    void addBim(std::shared_ptr<threepp::Object3D> bim_model);
    dragon::Gizmo* getGizmo() const;
protected:
    std::queue<std::function<void()>> fnc_queue{};
    std::unique_ptr<threepp::Raycaster> m_RayCaster{ nullptr };
    std::shared_ptr<dragon::Gizmo> m_Gizmo{ nullptr };
    std::shared_ptr<mbgl::LayerGroupBase> m_LayerGroup{ nullptr }; 
    threepp::Vector2 m_nor_pos{ -threepp::Infinity<float>, -threepp::Infinity<float> };
    Plane chooseBestPlane(const threepp::Vector3& gizmoOrigin,
        const threepp::Vector3& selectedAxis,
        const threepp::Vector3& cameraDir);
    /*HARD CODE*/
    bool isDrag{ false }; 
    static mbgl::Point<double> project(const mbgl::LatLng& c, const mbgl::TransformState& s);
    mbgl::CanonicalTileID root_tile_id{ 0,0,0 };
protected:
};
