#include "DrawPass.hpp"
#include "threepp/threepp.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
namespace dragon
{
    DrawPass::DrawPass(const int& width, const int& height)
    {
        if (!m_Scene)
            m_Scene = std::make_unique<threepp::Scene>(); 
        threepp::GLRenderTarget::Options opts;
        opts.format = threepp::Format::RGBA;
        opts.anisotropy = 16;
        if (!m_RenderTarget)
        {
            m_RenderTarget = std::make_unique<threepp::GLRenderTarget>(width, height, opts);
        }

    }
    void DrawPass::setSize(const int& width, const int& height)
    {
        if (m_RenderTarget)
            m_RenderTarget->setSize(width, height); 
    }
    threepp::GLRenderTarget* DrawPass::getRenderTarget()
    {
        return m_RenderTarget.get(); 
    }
    threepp::Scene* DrawPass::getScene()
    {
        return m_Scene.get(); 
    }
} 