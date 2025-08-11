#include "THREEPPSceneBuilder.hpp"
#include <threepp/threepp.hpp>

namespace dragon
{
	void SceneBuilder::IFCBuildScene(threepp::Group* container, threepp::Scene* scene, threepp::Camera* camera)
	{
		/*BUILD LIGHT*/
		std::shared_ptr<threepp::Group> light_group = threepp::Group::create(); 
		light_group->name = "light"; 
		std::shared_ptr<threepp::AmbientLight> a_light = threepp::AmbientLight::create(0xffffee, 0.4);
		a_light->name = "a_light"; 
		light_group->add(a_light);
		std::shared_ptr<threepp::Light> directionalLight1 = threepp::DirectionalLight::create(0xffeeff, 0.4);
		directionalLight1->position.set(1, 1, 1);
		directionalLight1->name = "d1_light"; 
		light_group->add(directionalLight1);
		std::shared_ptr<threepp::Light> directionalLight2 = threepp::DirectionalLight::create(0xffeeff, 0.4);
		directionalLight2->position.set(-1, 0.5, -1);
		directionalLight2->name = "d2_light"; 
		light_group->add(directionalLight2);
		/*BUILD CAMERA*/
		camera->position.z = 10;
		threepp::Box3 box{};
		box.setFromObject(*container);
		auto center = box.getCenter();
		camera->lookAt(center);
		scene->add(light_group); 
	}
}