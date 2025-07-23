#pragma once
#ifndef _MAIN_DRAW_PASS_HPP_
#define _MAIN_DRAW_PASS_HPP_
#include "DrawPass.hpp"
namespace dragon
{
	class OutLinePass : public DrawPass
	{
	public:
		OutLinePass(const int& width, const int& height);
	public:
		void render(threepp::GLRenderer* renderer, threepp::Camera* camera) override;
		void clear() override;
	public:
		void applyUniform(threepp::Texture* texture);
	private:
		std::shared_ptr<threepp::RawShaderMaterial> sobel_material{ nullptr };
		std::shared_ptr<threepp::Mesh> outline_mesh{ nullptr };
		std::shared_ptr<threepp::BufferGeometry> default_geometry{ nullptr };
	};
}
#endif // !_MAIN_DRAW_PASS_HPP_