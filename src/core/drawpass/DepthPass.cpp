#include "DepthPass.hpp"
#include "threepp/threepp.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
#include "threepp/materials/RawShaderMaterial.hpp"
#include "core/utils/StringHelper.hpp"
#include "config/app_config.hpp"
#include "core/Paths.hpp"
namespace dragon
{
	DepthPass::DepthPass(const int& width, const int& height) : DrawPass(width, height)
	{
		m_Scene->background = default_color::scene_depth_color;
		if (!depth_material)
		{
			depth_material = threepp::RawShaderMaterial::create();
			const std::string vertex_path = assets::Shader + "depth.vert";
			const std::string frag_path = assets::Shader + "depth.frag";
			std::string vertexSource{};
			std::string fragSource{};
			StringHelper::readFile(vertex_path, vertexSource);
			StringHelper::readFile(frag_path, fragSource);
			if (!vertexSource.empty() && !fragSource.empty())
			{
				depth_material->vertexShader = vertexSource;
				depth_material->fragmentShader = fragSource;
			}
		}
		opts.format = threepp::Format::RGBA;
		opts.type = threepp::Type::UnsignedShort;
		opts.minFilter = threepp::Filter::Nearest;
		opts.magFilter = threepp::Filter::Nearest;
		opts.anisotropy = 16;
		if (!m_RenderTarget)
		{
			m_RenderTarget = std::make_unique<threepp::GLRenderTarget>(width, height, opts);
		}
	}

	void DepthPass::render(threepp::GLRenderer* renderer, threepp::Camera* camera)
	{
		renderer->setRenderTarget(m_RenderTarget.get());
		renderer->render(*m_Scene, *camera);
		renderer->setRenderTarget(nullptr);
	}
	void DepthPass::debugRender(threepp::GLRenderer* renderer, threepp::Camera* camera)
	{
		renderer->render(*m_Scene, *camera);
	}
	void DepthPass::clear()
	{
		if (!default_geometry)
			default_geometry = threepp::BufferGeometry::create();
		m_Scene->clear(); 
		depth_meshes.clear(); 
		//depth_mesh->setGeometry(default_geometry);
	}
	void DepthPass::applyUniform(float nearPlane, float farPlane)
	{
		depth_material->uniforms["near"].setValue(nearPlane);
		depth_material->uniforms["far"].setValue(farPlane);
	}
	void DepthPass::updateOneDepthMeshSelect(std::shared_ptr<threepp::BufferGeometry> bufferGeometry)
	{
		//depth_mesh->setGeometry(bufferGeometry); 
		if (depth_meshes.size() != 1)
		{
			m_Scene->clear(); 
			depth_meshes.clear(); 
		}
		if (depth_meshes.empty())
		{
			std::shared_ptr<threepp::Mesh> new_mesh = threepp::Mesh::create(bufferGeometry,depth_material); 
			depth_meshes.emplace_back(new_mesh);
			m_Scene->add(new_mesh); 
		}
		/*UPDATE GEOMETRY FOR ELEMENT 0*/
		depth_meshes[0]->geometry()->dispose(); 
		depth_meshes[0]->setGeometry(bufferGeometry); 
	}
	void DepthPass::updateMultiDepthMeshSelect(std::shared_ptr<threepp::BufferGeometry> bufferGeometry)
	{
		std::shared_ptr<threepp::Mesh> new_mesh = threepp::Mesh::create(bufferGeometry,depth_material);
		depth_meshes.emplace_back(new_mesh);
		m_Scene->add(new_mesh); 
	}
	void DepthPass::reCreateRenderTarget(const int& width, const int& height)
	{
		m_RenderTarget->dispose();
		m_RenderTarget.reset();
		m_RenderTarget = nullptr;
		m_RenderTarget = std::make_unique<threepp::GLRenderTarget>(width, height, opts);
	}
	threepp::GLRenderTarget* DepthPass::getRenderTarget()
	{
		return m_RenderTarget.get();
	}
}