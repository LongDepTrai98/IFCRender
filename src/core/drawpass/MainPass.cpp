#include "MainPass.hpp"
#include "threepp/threepp.hpp"
#include "config/app_config.hpp"
namespace dragon
{
	MainPass::MainPass(const int& width, const int& height) : DrawPass(width, height)
	{
		m_Scene->background = default_color::scene_color;
	}
	void MainPass::render(threepp::GLRenderer* renderer, threepp::Camera* camera)
	{
		renderer->render(*m_Scene.get(), *camera);
	}
	void MainPass::clear()
	{
		m_Scene->clear();
	}
}