#include "ViewportGizmo.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
#include "core/Paths.hpp"
#include "threepp/geometries/EdgesGeometry.hpp"
#include <spdlog/spdlog.h>

namespace dragon
{
	ViewPortGizmo::ViewPortGizmo(RenderCanvas* canvas) : IRenderer(canvas)
	{
		m_Canvas->activeContext();
		m_Viewport_Size = { 100,100 }; 
		wxSize canvas_size = canvas->getSize();
		initCamera(m_Viewport_Size);
		initScene(m_Viewport_Size);
		m_Camera->position.z = 8;
		createCubeMesh(*m_Scene); 
		m_Scene->add(createLight());
		m_Canvas->deactiveContext();
	}
	ViewPortGizmo::~ViewPortGizmo()
	{
	}
	void ViewPortGizmo::resize(const int& width, const int& height)
	{
		if (m_Camera)
		{
			m_Camera->aspect = m_Viewport_Size.aspect();
			m_Camera->updateProjectionMatrix();
		}
	}
	void ViewPortGizmo::update(const float& dtTime)
	{
	}
	void ViewPortGizmo::render(threepp::GLRenderer* renderer)
	{
		if (renderer
			&& m_Scene
			&& m_Camera)
		{
			renderer->clearDepth(); 
			renderer->setViewport({ m_Padding, m_Padding, m_Viewport_Size.width(), m_Viewport_Size.height() });
			renderer->render(*m_Scene, *m_Camera);
		}
	}
	void ViewPortGizmo::handleRaycast(threepp::Vector2& nor_mouse_pos)
	{
		/*HANDLE RAY CASTER*/
	}
	void ViewPortGizmo::clearScene()
	{
	}
	std::shared_ptr<threepp::BufferGeometry> ViewPortGizmo::loadCubeGeometry()
	{
		threepp::STLLoader loader; 
		const std::string& cube_path = assets::Cube; 
		spdlog::info("Load model {}", cube_path); 
		return loader.load(cube_path); 
	}
	void ViewPortGizmo::createCubeMesh(threepp::Scene& scene)
	{
		std::shared_ptr<threepp::BufferGeometry> cube = loadCubeGeometry();
		cube->center();
		auto material = threepp::MeshPhongMaterial::create({ {"flatShading", true}, {"color", threepp::Color::lightgray} });
		auto mesh = threepp::Mesh::create(cube, material);
		mesh->position.set(0.0f, 0.0f, 0.0f);
		mesh->scale.set(0.2f, 0.2f, 0.2f);
		const float thresholdAngle = 30.0f;
		std::shared_ptr<threepp::EdgesGeometry> edge_geo = threepp::EdgesGeometry::create(*cube, thresholdAngle);
		std::shared_ptr<threepp::LineBasicMaterial> outline_material = threepp::LineBasicMaterial::create();
		outline_material->color = threepp::Color::darkslategray;
		std::shared_ptr<threepp::LineSegments> outlineEdge = threepp::LineSegments::create(edge_geo, outline_material);
		outlineEdge->position.set(0.0f, 0.0f, 0.0f);
		outlineEdge->scale.set(0.2f, 0.2f, 0.2f);
		auto group = threepp::Group::create(); 
		group->add(mesh); 
		group->add(outlineEdge); 
		scene.add(group); 
	}
	std::shared_ptr<threepp::Light> ViewPortGizmo::createLight()
	{
		return threepp::HemisphereLight::create(threepp::Color::white, threepp::Color::grey);
	}
	void ViewPortGizmo::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(60, w_size.aspect());
	}
	void ViewPortGizmo::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>();
	}
}