#include "DepthPass.hpp"
#include "threepp/threepp.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
namespace dragon
{
	DepthPass::DepthPass(const int& width, const int& height) : DrawPass(width, height)
	{
		m_Scene->background = threepp::Color::black;
		/*auto spriteMaterial = threepp::MeshBasicMaterial::create({ {"map", m_RenderTarget->texture}});
		auto sprite = threepp::Mesh::create(threepp::PlaneGeometry::create(), spriteMaterial);
		m_RenderTarget->depthBuffer = true; 
		sprite->scale.set(2, 2, 1);
		m_Scene->add(sprite); */
	}
}


