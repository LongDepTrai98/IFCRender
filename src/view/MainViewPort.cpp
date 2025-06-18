#include "MainViewPort.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
namespace dragon
{
	static void createAndAddLights(threepp::Scene& scene) {
		auto light1 = threepp::AmbientLight::create(0xffffff, 1.f);
		scene.add(light1);
	}

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
	MainViewPort::MainViewPort(RenderCanvas* canvas) : IRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Viewport_Size = { canvas_size.x, canvas_size.y }; 
		initScene(m_Viewport_Size);
		initCamera(m_Viewport_Size);
		createAndAddLights(*m_Scene); 
		createExampleScene(); 
	}
	MainViewPort::~MainViewPort()
	{
	}
	void MainViewPort::createExampleScene()
	{
		m_Camera->position.z = 5;
		//auto box = example::createBox();
		//m_Scene->add(box);
		//auto sphere = example::createSphere();
		//box->add(sphere);
		//auto plane = example::createPlane();
		//auto planeMaterial = plane->material()->as<threepp::MeshBasicMaterial>();
		//m_Scene->add(plane);
	}
	void MainViewPort::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(75, w_size.aspect()); 
	}
	void MainViewPort::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>(); 
		m_Scene->background = threepp::Color::aliceblue; 
	}
	void MainViewPort::resize(const int& width, const int& height)
	{
		m_Viewport_Size = { width,height }; 
		if (m_Camera)
		{
			m_Camera->aspect = m_Viewport_Size.aspect(); 
			m_Camera->updateProjectionMatrix(); 
		}
	}
	void MainViewPort::update(const float& dtTime)
	{
	}
	void MainViewPort::render(threepp::GLRenderer* renderer)
	{
		if (m_Renderer
			&& m_Scene
			&& m_Camera)
		{
			renderer->setViewport({0,0,m_Viewport_Size.width(),m_Viewport_Size.height()});
			renderer->render(*m_Scene.get(), *m_Camera.get()); 
		}
	}
}