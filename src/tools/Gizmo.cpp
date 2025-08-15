#include "Gizmo.hpp"
#include "threepp/helpers/PlaneHelper.hpp"
#include "threepp/threepp.hpp"
namespace dragon
{
	std::shared_ptr<threepp::Group> Gizmo::create(threepp::Object3D* target_)
	{
		target = target_; 
		std::shared_ptr<threepp::Group> gizmo = threepp::Group::create(); 
		//CREATE ARROW OX, PLANE OX
		threepp::Vector3 ox_dir{ 1.0,0.0,0.0 }; 
		std::shared_ptr<threepp::ArrowHelper> arrow_ox = threepp::ArrowHelper::create(
			ox_dir, 
			{ 0, 0, 0 },
			arrow_length,
			threepp::Color::red,
			arrow_head_length,
			arrow_head_width);
		arrow_ox->name = "arrow_x"; 
		const auto planeXYHelper = createXYPlaneHelper(plane_size);
		planeXYHelper->name = "plane_xy";
		//CREATE ARROW OY 
		threepp::Vector3 oy_dir{ 0.0,1.0,0.0 }; 
		std::shared_ptr<threepp::ArrowHelper> arrow_oy = threepp::ArrowHelper::create(
			oy_dir,
			{ 0, 0, 0 },
			arrow_length,
			threepp::Color::green,
			arrow_head_length,
			arrow_head_width); 
		arrow_oy->name = "arrow_y";
		threepp::Plane plane_oy(oy_dir, 0.0); 
		const auto planeYZHelper = createYZPlaneHelper(plane_size);
		planeXYHelper->name = "plane_yz";
		//CREATE ARROW OZ 
		threepp::Vector3 oz_dir{ 0.0,0.0,1.0 }; 
		std::shared_ptr<threepp::ArrowHelper> arrow_oz = threepp::ArrowHelper::create(
			oz_dir,
			{ 0, 0, 0 }, 
			arrow_length,
			threepp::Color::blue, 
			arrow_head_length,
			arrow_head_width);
		arrow_oz->name = "arrow_z";
		const auto planeXZHelper = createXZPlaneHelper(plane_size);
		planeXZHelper->name = "plane_xz";
		gizmo->add(arrow_ox); 
		gizmo->add(arrow_oy); 
		gizmo->add(arrow_oz); 
		gizmo->add(planeXYHelper); 
		gizmo->add(planeXZHelper);
		gizmo->add(planeYZHelper);
		//CAL CENTER OF OBJECT 
		threepp::Box3 box{};
		box.setFromObject(*target_);
		auto center = box.getCenter();
		gizmo->position.set(center.x,center.y,center.z); 
		return gizmo;
	}
	std::shared_ptr<threepp::Mesh> Gizmo::createXYPlaneHelper(float size)
	{
		auto geometry = threepp::PlaneGeometry::create(size, size);
		auto material = threepp::MeshBasicMaterial::create();
		auto pos = size * 0.5f;
		material->color = threepp::Color::yellow; 
		material->side = threepp::Side::Double;
		auto plane = threepp::Mesh::create(geometry, material);
		plane->position.set(pos + padding_plane, pos + padding_plane, 0.0);
		return plane;
	}
	std::shared_ptr<threepp::Mesh> Gizmo::createXZPlaneHelper(float size)
	{
		auto geometry = threepp::PlaneGeometry::create(size, size);
		auto material = threepp::MeshBasicMaterial::create();
		auto pos = size * 0.5f;
		material->color = threepp::Color::violet; 
		material->side = threepp::Side::Double;
		auto plane = threepp::Mesh::create(geometry, material);
		plane->rotation.x = threepp::math::degToRad(90);;
		plane->position.set(pos + padding_plane, 0.0, pos + padding_plane);
		return plane;
	}
	std::shared_ptr<threepp::Mesh> Gizmo::createYZPlaneHelper(float size)
	{
		auto geometry = threepp::PlaneGeometry::create(size, size);
		auto pos = size * 0.5f; 
		auto material = threepp::MeshBasicMaterial::create();
		material->color = threepp::Color::lightblue;
		material->side = threepp::Side::Double;
		auto plane = threepp::Mesh::create(geometry, material);
		plane->rotation.y = threepp::math::degToRad(90);;
		plane->position.set(0.0, pos + padding_plane, pos + padding_plane);
		return plane;
	}
}