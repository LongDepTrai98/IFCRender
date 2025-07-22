#include "DrawPass.hpp"
#include "threepp/threepp.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
namespace dragon
{
	DrawPass::DrawPass(const int& width, const int& height)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>();
	}
	DrawPass::~DrawPass()
	{
		/*  if (m_RenderTarget)
		  {
			  m_RenderTarget->texture->dispose();
			  m_RenderTarget->dispose();
		  }*/
	}
	threepp::Scene* DrawPass::getScene()
	{
		return m_Scene.get();
	}
}