#pragma once
#ifndef _DEPTH_PASS_HPP_
#define _DEPTH_PASS_HPP_
#include "DrawPass.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
namespace dragon
{
	class DepthPass : public DrawPass
	{
	public:
		DepthPass(const int& width, const int& height);
	public:
		void render(threepp::GLRenderer* renderer, threepp::Camera* camera) override;
		void debugRender(threepp::GLRenderer* renderer, threepp::Camera* camera);
		void clear() override;
		void applyUniform(float near, float far);
		void updateOneDepthMeshSelect(std::shared_ptr<threepp::BufferGeometry> bufferGeometry);
		void updateMultiDepthMeshSelect(std::shared_ptr<threepp::BufferGeometry> bufferGeometry);
		void reCreateRenderTarget(const int& width, const int& height);
		threepp::GLRenderTarget* getRenderTarget();
	private:
		threepp::GLRenderTarget::Options opts;
		std::shared_ptr<threepp::GLRenderTarget> m_RenderTarget{ nullptr };
		std::shared_ptr<threepp::RawShaderMaterial> depth_material{ nullptr };
		std::shared_ptr<threepp::BufferGeometry> default_geometry{ nullptr };
	};
}
#endif // !_DEPTH_PASS_HPP_