#ifndef _SCENE_BUILDER_HPP_
#define _SCENE_BUILDER_HPP_
namespace threepp
{
	class Group;
	class Scene;
	class Camera;
}
namespace dragon
{
	class SceneBuilder
	{
	public:
		static void IFCBuildScene(threepp::Group* container,
			threepp::Scene* scene,
			threepp::Camera* camera);
	private:
		SceneBuilder() = default;
	};
}
#endif // !_SCENE_BUILDER_HPP_