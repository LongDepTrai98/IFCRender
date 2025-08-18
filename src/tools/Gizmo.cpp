#include "Gizmo.hpp"
#include "threepp/helpers/PlaneHelper.hpp"
#include "threepp/threepp.hpp"
#include <spdlog/spdlog.h>
namespace dragon
{
	std::shared_ptr<threepp::Group> Gizmo::create()
	{
		gizmo = threepp::Group::create(); 
		gizmo->name = "gizmo"; 
		threepp::Vector3 ox_dir{ 1.0,0.0,0.0 };
		threepp::Vector3 oy_dir{ 1.0,1.0,0.0 }; 
		threepp::Vector3 oz_dir{ 0.0,0.0,1.0 }; 
		auto makeArrow = [&](const threepp::Color& color, const threepp::Vector3& dir, const std::string& name) {
			std::string cone_name = "cone_" + name; 
			std::string cyl_name = "cyl_" + name; 
			auto group = threepp::Group::create();
			auto length = arrow_length; 
			auto thickness = 1.0f; 
			// Thân mũi tên (cylinder)
			auto cylGeo = threepp::CylinderGeometry::create(thickness, thickness, length, 8);
			auto cylMat = threepp::MeshBasicMaterial::create();
			cylMat->color = color;
			cylMat->side = threepp::Side::Double;
			cylMat->depthTest = false; 
			auto cyl = threepp::Mesh::create(cylGeo, cylMat);
			cyl->position.y = length / 2.0f;
			cyl->name = cyl_name; 
			group->add(cyl);
			auto coneGeo = threepp::ConeGeometry::create(thickness * 5, length * 0.4f, 12);
			auto coneMat = threepp::MeshBasicMaterial::create();
			coneMat->color = color;
			coneMat->side = threepp::Side::Double;
			coneMat->depthTest = false; 
			auto cone = threepp::Mesh::create(coneGeo, coneMat);
			cone->position.y = length + (length * 0.1f);
			cone->name = cone_name; 
			group->add(cone);
			// Quay group theo hướng trục
			if (dir.equals(threepp::Vector3(1, 0, 0))) {
				group->rotation.z = -threepp::math::PI / 2;
			}
			else if (dir.equals(threepp::Vector3(0, 0, 1))) {
				group->rotation.x = threepp::math::PI / 2;
			}
			group->name = name; 
			return group;
		};
		const auto planeXYHelper = createXYPlaneHelper(plane_size);
		planeXYHelper->name = "plane_xy"; 
		const auto planeYZHelper = createYZPlaneHelper(plane_size);
		planeYZHelper->name = "plane_yz"; 
		const auto planeXZHelper = createXZPlaneHelper(plane_size);
		planeXZHelper->name = "plane_xz"; 
		//ox
		gizmo->add(makeArrow(threepp::Color::red, threepp::Vector3(1, 0, 0), "ox"));
		// oy
		gizmo->add(makeArrow(threepp::Color::green, threepp::Vector3(0, 1, 0), "oy"));
		// oz
		gizmo->add(makeArrow(threepp::Color::blue, threepp::Vector3(0, 0, 1), "oz"));
		//plane x
		gizmo->add(planeXYHelper); 
		gizmo->add(planeXZHelper); 
		gizmo->add(planeYZHelper); 
		return gizmo;
	}
	std::shared_ptr<threepp::Mesh> Gizmo::createXYPlaneHelper(float size)
	{
		auto geometry = threepp::PlaneGeometry::create(size, size);
		auto material = threepp::MeshBasicMaterial::create();
		auto pos = size * 0.5f;
		material->color = threepp::Color::yellow; 
		material->side = threepp::Side::Double;
		material->depthTest = false;
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
		material->depthTest = false; 
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
		material->depthTest = false;
		auto plane = threepp::Mesh::create(geometry, material);
		plane->rotation.y = threepp::math::degToRad(90);;
		plane->position.set(0.0, pos + padding_plane, pos + padding_plane);
		return plane;
	}
	void Gizmo::setTarget(threepp::Object3D* target_)
	{
		target = target_; 
		threepp::Box3 box{};
		box.setFromObject(*target_);
		auto center = box.getCenter();
		gizmo->position.set(center.x, center.y, center.z);
	}
	void Gizmo::startDrag(threepp::Ray& ray, threepp::Vector3& camDirection, threepp::Vector3& selected_axis_, bool isAxis_)
	{
		selected_axis = selected_axis_; 
		threepp::Vector3 planeNormal;
		isAxis = isAxis_;
		if (isAxis)
		{
			threepp::Vector3 u = camDirection.clone().cross(selected_axis);
			planeNormal = u.cross(selected_axis).normalize();
		}
		else
		{
			planeNormal = selected_axis; 
		}
		dragPlane.setFromNormalAndCoplanarPoint(planeNormal,gizmo->position); 
		ray.intersectPlane(dragPlane, startPoint); 
		spdlog::info("Start Drag: {}, {}, {}", startPoint.x,startPoint.y, startPoint.z); 
	}
	void Gizmo::updateDrag(threepp::Ray& ray)
	{
		threepp::Vector3 hitPointNow; 
		ray.intersectPlane(dragPlane, hitPointNow); 
		threepp::Vector3 delta = hitPointNow - startPoint;
		threepp::Vector3 translation; 
		if (isAxis)
		{
			float moveAmount = delta.dot(selected_axis.normalize());
			translation = selected_axis.normalize() * moveAmount;
		}
		else
		{
			translation = delta; 
		}
		spdlog::info("Translation delta : {}, {} , {}", translation.x, translation.y, translation.z); 
		startPoint = hitPointNow; 
		//update ui 
		threepp::Matrix4 translationMatrix;
		translationMatrix.makeTranslation(translation);
		gizmo->applyMatrix4(translationMatrix); 
		target->applyMatrix4(translationMatrix); 
	}
	void Gizmo::endDrag()
	{
		selected_axis = threepp::Vector3(0.0, 0.0, 0.0); 
		spdlog::info("End drag"); 
	}
}