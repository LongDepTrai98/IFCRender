#ifndef _SCENE_CONTEXT_THREEPP_
#define _SCENE_CONTEXT_THREEPP_
#include "threepp/threepp.hpp"
namespace dragon
{
	class SceneContext
	{
	public:
		virtual ~SceneContext() = default;
	public:
		threepp::Camera* getCamera()
		{
			return m_Camera.get();
		}
	protected:
		std::unique_ptr<threepp::Scene> m_Scene{ nullptr };
		std::unique_ptr<threepp::PerspectiveCamera> m_Camera{ nullptr };
	};
}
#endif // !_SCENE_CONTEXT_THREEPP_