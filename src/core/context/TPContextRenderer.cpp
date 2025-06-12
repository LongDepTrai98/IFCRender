#include "TPContextRenderer.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/threepp.hpp"
/*
* THREEPP CONTEXT RENDERER
* BACKEND RENDERER IS OPENGL + THREEPP 
*/
namespace dragon
{
	namespace example
	{
	using namespace threepp; 
	auto createBox() {

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

	auto createSphere() {

		const auto sphereGeometry = SphereGeometry::create(0.5f);
		const auto sphereMaterial = MeshBasicMaterial::create();
		sphereMaterial->color.setHex(0x00ff00);
		sphereMaterial->wireframe = true;
		auto sphere = Mesh::create(sphereGeometry, sphereMaterial);
		sphere->position.setX(-1);

		return sphere;
	}

	auto createPlane() {

		const auto planeGeometry = PlaneGeometry::create(5, 5);
		const auto planeMaterial = MeshBasicMaterial::create();
		planeMaterial->color.setHex(Color::yellow);
		planeMaterial->transparent = true;
		planeMaterial->opacity = 0.5f;
		planeMaterial->side = Side::Double;
		auto plane = Mesh::create(planeGeometry, planeMaterial);
		plane->position.setZ(-2);

		return plane;
	}
	}


	TPContextRenderer::TPContextRenderer(RenderCanvas* canvas) : IContextRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Canvas->activeContext(); 
		//create windowsize 
		threepp::WindowSize window_size(canvas_size.x, canvas_size.y);
		initRenderer(window_size);
		initScene(window_size);
		initCamera(window_size); 
		createExampleScene();
		m_Canvas->deactiveContext(); 
	}
	TPContextRenderer::~TPContextRenderer()
	{
	}
	void TPContextRenderer::initRenderer(threepp::WindowSize& w_size)
	{
		if (!m_Renderer)
			m_Renderer = std::make_unique<threepp::GLRenderer>(w_size);
	}
	void TPContextRenderer::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(75, w_size.aspect());
	}
	void TPContextRenderer::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>(); 
		m_Scene->background = threepp::Color::aliceblue; 
	}
	void TPContextRenderer::ctxRender()
	{
		if (m_Renderer
			&& m_Scene
			&& m_Camera)
		{
			m_Renderer->render(*m_Scene, *m_Camera); 

		}
	}
	void TPContextRenderer::createExampleScene()
	{
		m_Camera->position.z = 5;
		auto box = example::createBox();
		m_Scene->add(box);
		auto sphere = example::createSphere();
		box->add(sphere);
		auto plane = example::createPlane();
		auto planeMaterial = plane->material()->as<threepp::MeshBasicMaterial>();
		m_Scene->add(plane);
	}
	void TPContextRenderer::resize(const int& width, const int& height)
	{
		threepp::WindowSize window_size(width,height);
		if (m_Camera)
		{
			m_Camera->aspect = window_size.aspect(); 
			m_Camera->updateProjectionMatrix(); 
			m_Renderer->setSize(window_size); 
		}
	}
	void TPContextRenderer::update(const float& dtTime)
	{
	}
	void TPContextRenderer::render()
	{
		if (m_Canvas)
		{
			/*
			* context render here 
			*/
			ctxRender();
		}
	}
}