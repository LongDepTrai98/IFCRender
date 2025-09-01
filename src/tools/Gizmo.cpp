#include "Gizmo.hpp"
#include "threepp/helpers/PlaneHelper.hpp"
#include "threepp/threepp.hpp"
#include "core/utils/ThreeHelper.hpp"
#include "threepp/core/Raycaster.hpp"
#include <spdlog/spdlog.h>
#include <iostream>
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
			if (dir.equals(threepp::Vector3(1, 0, 0))) {
				group->rotation.z = -threepp::math::PI / 2;
			}
			else if (dir.equals(threepp::Vector3(0, 0, 1))) {
				group->rotation.x = threepp::math::PI / 2;
			}
			group->name = name; 
			return group;
		};
		auto makeRingZ = [&]() {
			auto geo = threepp::TorusGeometry::create(100.0f, 5.0f, 16, 64);
			auto mat = threepp::MeshBasicMaterial::create();
			mat->color = threepp::Color::blue;
			mat->transparent = true;
			mat->opacity = 0.8f;
			mat->depthTest = false;
			mat->depthWrite = false;
			auto mesh = threepp::Mesh::create(geo, mat);
			return mesh;
		};
		std::shared_ptr<threepp::Group> translate_group = threepp::Group::create(); 
		const auto planeXYHelper = createXYPlaneHelper(plane_size);
		planeXYHelper->name = "plane_xy"; 
		const auto planeYZHelper = createYZPlaneHelper(plane_size);
		planeYZHelper->name = "plane_yz"; 
		const auto planeXZHelper = createXZPlaneHelper(plane_size);
		planeXZHelper->name = "plane_xz"; 
		//ox
		translate_group->add(makeArrow(threepp::Color::red, threepp::Vector3(1, 0, 0), "ox"));
		// oy
		translate_group->add(makeArrow(threepp::Color::green, threepp::Vector3(0, 1, 0), "oy"));
		// oz
		translate_group->add(makeArrow(threepp::Color::blue, threepp::Vector3(0, 0, 1), "oz"));
		//plane x
		translate_group->add(planeXYHelper);
		translate_group->add(planeXZHelper);
		translate_group->add(planeYZHelper);
		translate_group->name = "translate_grp"; 
		gizmo->add(translate_group); 
		std::shared_ptr<threepp::Group> rotate_group = threepp::Group::create();
		auto ringZ = makeRingZ(); 
		ringZ->name = "ring_z";
		rotate_group->add(ringZ);
		rotate_group->name = "rotate_grp"; 
		gizmo->add(rotate_group); 
		switchMode(MODE::TRANSLATE); 
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
		std::cout << box.min().x << " " << box.max().x << std::endl; 
		std::cout << box.min().y << " " << box.max().y << std::endl; 
		threepp::Vector3 target_pos; 
		threepp::Quaternion target_quaternion; 
		threepp::Vector3 target_scale; 
		threepp::Matrix4 rotate_matrix; 
		rotate_matrix.makeRotationFromQuaternion(target_quaternion); 
		gizmo->position.set(center.x, center.y, center.z);
		//gizmo->applyMatrix4(rotate_matrix); 
	}
	void Gizmo::startDrag(threepp::Raycaster* rayCaster, threepp::Vector3& camDirection)
	{
		if (gizmo->visible == false)
		{
			dragging = false; 
			return; 
		}
		const auto intersects = rayCaster->intersectObjects(gizmo->children, true);
		if (intersects.size() != 0) {
			threepp::Vector3 selected_axis{ 0.0,0.0,0.0 };
			std::string obj_name = intersects[0].object->name;
			bool isAxis{ false };
			if (obj_name == "cone_ox")
			{
				selected_axis = threepp::Vector3(1.0, 0.0, 0.0);
				isAxis = true;
			}
			else if (obj_name == "cone_oy")
			{
				selected_axis = threepp::Vector3(0.0, 1.0, 0.0);
				isAxis = true;
			}
			else if (obj_name == "cone_oz")
			{
				selected_axis = threepp::Vector3(0.0, 0.0, 1.0);
				isAxis = true;
			}
			else if (obj_name == "plane_xy")
			{
				selected_axis = threepp::Vector3(0.0, 0.0, 1.0);
			}
			else if (obj_name == "plane_yz")
			{
				selected_axis = threepp::Vector3(1.0, 0.0, 0.0);
			}
			else if (obj_name == "plane_xz")
			{
				selected_axis = threepp::Vector3(0.0, 1.0, 0.0);
			}
			else if (obj_name == "ring_z")
			{
				selected_axis = threepp::Vector3(0.0, 0.0, 1.0);
				/*threepp::Plane plane(threepp::Vector3(0, 0, 1), -gizmo->position.dot(threepp::Vector3(0, 0, 1)));
				auto plane_helper = threepp::PlaneHelper::create(plane, 200); */
				//scene->add(plane_helper); 
			} 
			else
			{
				dragging = false; 
				return;
			}
			if (currentMode == MODE::TRANSLATE)
			{
				ss.isAxis = isAxis;
				threepp::Vector3 planeNormal;
				if (isAxis)
				{
					threepp::Vector3 u = camDirection.clone().cross(selected_axis);
					planeNormal = u.cross(selected_axis).normalize();
				}
				else
				{
					planeNormal = selected_axis;
				}
				ss.planeNormalWorld.copy(planeNormal); 
				ss.dragPlane.setFromNormalAndCoplanarPoint(planeNormal, gizmo->position);
				rayCaster->ray.intersectPlane(ss.dragPlane, ss.startPoint);
			}
			else if (currentMode == MODE::ROTATE)
			{
				rs.startObjQuat.copy(target->quaternion);
				rs.ringNormalWorld.copy(selected_axis);
				rs.totalAngle = 0.0f;
				rs.vPrev.z = 0;
				threepp::Vector3 hit;
				threepp::Vector3 gizmo_worldpos;
				gizmo->getWorldPosition(gizmo_worldpos);
				threepp::Plane plane(rs.ringNormalWorld, -rs.ringNormalWorld.dot(gizmo_worldpos));
				rayCaster->ray.intersectPlane(plane, hit);
				rs.vPrev = hit.sub(gizmo_worldpos).normalize();
			}
			dragging = true;
		}
	}
	void Gizmo::updateDrag(threepp::Raycaster* rayCaster, threepp::Vector3 camDir)
	{
		auto ray = rayCaster->ray; 
		if (currentMode == MODE::TRANSLATE)
		{
			threepp::Vector3 hitPointNow;
			ray.intersectPlane(ss.dragPlane, hitPointNow);
			threepp::Vector3 delta = hitPointNow - ss.startPoint;
			threepp::Vector3 translation;
			if (ss.isAxis)
			{
				float moveAmount = delta.dot(ss.planeNormalWorld.normalize());
				translation = ss.planeNormalWorld.normalize() * moveAmount;
			}
			else
			{
				translation = delta;
			}
			spdlog::info("Translation delta : {}, {} , {}", translation.x, translation.y, translation.z);
			ss.startPoint = hitPointNow;
			//update ui 
			threepp::Matrix4 translationMatrix;
			translationMatrix.makeTranslation(translation);
			gizmo->applyMatrix4(translationMatrix);
			target->applyMatrix4(translationMatrix);
		} else if (currentMode == MODE::ROTATE)
		{
			threepp::Vector3 gizmo_worldpos; 
			gizmo->getWorldPosition(gizmo_worldpos); 
			threepp::Plane plane(rs.ringNormalWorld, -rs.ringNormalWorld.dot(gizmo_worldpos));
			threepp::Vector3 hit; 
			ray.intersectPlane(plane,hit);
			threepp::Vector3 v1 = hit.clone().sub(gizmo_worldpos);
			v1.z = 0;    
			v1.normalize();
			float delta = std::atan2(
				threepp::Vector3(0, 0, 1).dot(rs.vPrev.clone().cross(v1)), 
				rs.vPrev.dot(v1)                                
			);
			float c; 
			delta < 0 ? c = -1.0f : c = 1.0f; 
			rs.totalAngle += delta; 
			rs.vPrev = v1;
			threepp::Matrix4 rotMat = ThreeHelper::createMatrixRotateAroundPivot(gizmo_worldpos, rs.ringNormalWorld, c * 0.05);
			gizmo->applyMatrix4(rotMat); 
			target->applyMatrix4(rotMat); 
		}
	}
	void Gizmo::switchMode(MODE mode)
	{
		auto switch_lambda = [&](MODE mode) {
			switch (mode)
			{
			case dragon::Gizmo::MODE::TRANSLATE:
			{
				if (auto translate_grp = gizmo->getObjectByName("translate_grp"))
				{
					translate_grp->visible = true; 
				}
				if (auto rotate_grp = gizmo->getObjectByName("rotate_grp"))
				{
					rotate_grp->visible = false; 
				}
				break;
			}
			case dragon::Gizmo::MODE::ROTATE:
			{
				if (auto translate_grp = gizmo->getObjectByName("translate_grp"))
				{
					translate_grp->visible = false;
				}
				if (auto rotate_grp = gizmo->getObjectByName("rotate_grp"))
				{
					rotate_grp->visible = true;
				}
				break;
			}
			default:
				break;
			}
		}; 
		currentMode = mode;
		switch_lambda(currentMode); 
	}
	void Gizmo::endDrag()
	{
		rs.totalAngle = 0; 
		dragging = false; 
	}
}