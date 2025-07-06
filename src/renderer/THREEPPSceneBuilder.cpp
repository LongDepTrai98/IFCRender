#include "THREEPPSceneBuilder.hpp"
#include <threepp/threepp.hpp>

namespace dragon
{
	void SceneBuilder::IFCBuildScene(threepp::Group* container, threepp::Scene* scene, threepp::Camera* camera)
	{
		/*BUILD LIGHT*/
		std::shared_ptr<threepp::AmbientLight> a_light = threepp::AmbientLight::create(0xffffee, 0.25);
		scene->add(a_light);
		std::shared_ptr<threepp::Light> directionalLight1 = threepp::DirectionalLight::create(0xffeeff, 0.6);
		directionalLight1->position.set(1, 1, 1);
		scene->add(directionalLight1);
		std::shared_ptr<threepp::Light> directionalLight2 = threepp::DirectionalLight::create(0xffffff, 0.6);
		directionalLight2->position.set(-1, 0.5, -1);
		scene->add(directionalLight2);
		/*BUILD CAMERA*/
		camera->position.z = 10;
		threepp::Box3 box{};
		box.setFromObject(*container);
		auto center = box.getCenter();
		//camera->lookAt(center);
		//camera->updateMatrix();
		auto min = box.min();
		auto max = box.max();
		/*BUILD PLANE*/
		//auto geometry = threepp::PlaneGeometry::create(150, 150);
		//auto material = threepp::MeshBasicMaterial::create();
		////material->opacity = 0.2f;
		////material->transparent = true;
		//auto mesh = threepp::Mesh::create(geometry, material);
		//mesh->rotation.x = -threepp::math::PI / 2;
		//auto grid = threepp::GridHelper::create(150, 150);
		//grid->rotation.x = threepp::math::PI / 2;
		//grid->position.z = min.z;
		//mesh->add(grid);
		//scene->add(mesh);
	}
}