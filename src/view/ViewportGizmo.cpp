#include "ViewportGizmo.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
namespace dragon
{
	namespace example
	{
		using namespace threepp;
		auto createBox2() {
			const auto boxGeometry = BoxGeometry::create();
			const auto boxMaterial = MeshBasicMaterial::create();
			boxMaterial->color.setRGB(1, 0, 0);
			boxMaterial->transparent = true;
			boxMaterial->opacity = 0.1f;
			auto box = Mesh::create(boxGeometry, boxMaterial);

			auto wiredBox = LineSegments::create(WireframeGeometry::create(*boxGeometry));
			wiredBox->material()->as<LineBasicMaterial>()->depthTest = false;
			wiredBox->material()->as<LineBasicMaterial>()->color = Color::gray;
			box->add(wiredBox);

			return box;
		}
	}
	ViewPortGizmo::ViewPortGizmo(RenderCanvas* canvas) : IRenderer(canvas)
	{
		m_Canvas->activeContext();
		m_Viewport_Size = { 50,50 }; 
		wxSize canvas_size = canvas->getSize();
		initCamera(m_Viewport_Size);
		initScene(m_Viewport_Size);
		m_Camera->position.z = 5;
		auto box = example::createBox2();
		m_Scene->add(box);
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
			renderer->setViewport({ m_Padding, m_Padding, m_Viewport_Size.width(), m_Viewport_Size.height() });
			renderer->render(*m_Scene, *m_Camera);
		}
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