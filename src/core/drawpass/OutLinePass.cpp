#include "OutLinePass.hpp"
#include "threepp/threepp.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
#include "threepp/materials/RawShaderMaterial.hpp"
#include "threepp/core/InterleavedBufferAttribute.hpp"
#include "config/app_config.hpp"
#include "core/Paths.hpp"
#include "core/utils/StringHelper.hpp"
namespace dragon
{
	OutLinePass::OutLinePass(const int& width, const int& height) : DrawPass(width, height)
	{
		if (!sobel_material)
		{
			sobel_material = threepp::RawShaderMaterial::create();
			const std::string vertex_path = assets::Shader + "outline.vert";
			const std::string frag_path = assets::Shader + "outline.frag";
			std::string vertexSource{};
			std::string fragSource{};
			StringHelper::readFile(vertex_path, vertexSource);
			StringHelper::readFile(frag_path, fragSource);
			if (!vertexSource.empty() && !fragSource.empty())
			{
				sobel_material->vertexShader = vertexSource;
				sobel_material->fragmentShader = fragSource;
				threepp::Vector4 outlineColor(default_color::outline_selected_color.r,
					default_color::outline_selected_color.g,
					default_color::outline_selected_color.b,
					0.8f);
				sobel_material->uniforms["colorOutline"].setValue(outlineColor);
				sobel_material->uniforms["threshold"].setValue(0.3f);
				sobel_material->uniforms["edgeSoftness"].setValue(0.05f);
			}
			sobel_material->transparent = true;
		}
		if (!outline_mesh)
		{
			std::vector<float> vertices{
			-1.f,  1.f, 0.f,  0.f, 1.f, // top-left
			-1.f, -1.f, 0.f,  0.f, 0.f, // bottom-left
			1.f, -1.f, 0.f,  1.f, 0.f, // bottom-right
			1.f,  1.f, 0.f,  1.f, 1.f  // top-right
			};
			auto quad_geometry = threepp::BufferGeometry::create();
			auto interleavedBuffer = threepp::InterleavedBuffer::create(vertices, 5);
			quad_geometry->setIndex(std::vector<int>{0, 1, 2, 0, 2, 3});
			quad_geometry->setAttribute("position", std::make_unique<threepp::InterleavedBufferAttribute>(interleavedBuffer, 3, 0, false));
			quad_geometry->setAttribute("uv", std::make_unique<threepp::InterleavedBufferAttribute>(interleavedBuffer, 2, 3, false));
			outline_mesh = threepp::Mesh::create(quad_geometry, sobel_material);
			m_Scene->add(outline_mesh);
		}
	}
	void OutLinePass::render(threepp::GLRenderer* renderer, threepp::Camera* camera)
	{
		sobel_material->depthTest = false;
		renderer->state().setBlending(threepp::Blending::Normal);
		renderer->render(*m_Scene, *camera);
		renderer->state().setBlending(threepp::Blending::None);
		sobel_material->depthTest = true;
	}
	void OutLinePass::clear()
	{
		if (!default_geometry)
			default_geometry = threepp::BufferGeometry::create();
		outline_mesh->setGeometry(default_geometry);
	}
	void OutLinePass::applyUniform(threepp::Texture* texture)
	{
		threepp::Vector2 texture_size((float)texture->image().width, (float)texture->image().height);
		sobel_material->uniforms["textureSize"].setValue(texture_size);
		sobel_material->uniforms["depthTex"].setValue(texture);
	}
}