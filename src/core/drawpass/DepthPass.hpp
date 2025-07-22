#pragma once
#ifndef _DEPTH_PASS_HPP_
#define _DEPTH_PASS_HPP_
#include "DrawPass.hpp"
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
		void updateDepthMeshSelect(std::shared_ptr<threepp::BufferGeometry> bufferGeometry);
		threepp::GLRenderTarget* getRenderTarget();
	private:
		std::shared_ptr<threepp::GLRenderTarget> m_RenderTarget{ nullptr };
		std::shared_ptr<threepp::RawShaderMaterial> depth_material{ nullptr };
		std::shared_ptr<threepp::BufferGeometry> default_geometry{ nullptr };
		std::shared_ptr<threepp::Mesh> depth_mesh{ nullptr };
	};
}
#endif // !_DEPTH_PASS_HPP_