#include "THREEPPRenderer.hpp"
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


	THREEPPRenderer::THREEPPRenderer(RenderCanvas* canvas) : IRenderer(canvas)
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
	THREEPPRenderer::~THREEPPRenderer()
	{
	}
	void THREEPPRenderer::initRenderer(threepp::WindowSize& w_size)
	{
		if (!m_Renderer)
			m_Renderer = std::make_unique<threepp::GLRenderer>(w_size);
	}
	void THREEPPRenderer::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(75, w_size.aspect());
	}
	void THREEPPRenderer::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>(); 
		m_Scene->background = threepp::Color::aliceblue; 
	}
	void THREEPPRenderer::initController()
	{
		if (!m_OrbitControls)
			m_OrbitControls = std::make_unique<threepp::OrbitControls>(*m_Camera, *this); 
	}
	void THREEPPRenderer::ctxRender()
	{
		if (m_Renderer
			&& m_Scene
			&& m_Camera)
		{
			m_Renderer->render(*m_Scene, *m_Camera); 

		}
	}
	void THREEPPRenderer::createExampleScene()
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
	void THREEPPRenderer::resize(const int& width, const int& height)
	{
		threepp::WindowSize window_size(width,height);
		if (m_Camera)
		{
			m_Camera->aspect = window_size.aspect(); 
			m_Camera->updateProjectionMatrix(); 
			m_Renderer->setSize(window_size); 
		}
	}
	void THREEPPRenderer::update(const float& dtTime)
	{
	}
	void THREEPPRenderer::render()
	{
		if (m_Canvas)
		{
			/*
			* context render here 
			*/
			ctxRender();
		}
	}
	void THREEPPRenderer::OnMouseMove(wxMouseEvent& event)
	{
		wxPoint pos = event.GetPosition();
		threepp::Vector2 mousePos(static_cast<float>(pos.x), static_cast<float>(pos.y));
		onMouseMoveEvent(mousePos);
		event.Skip();
	}
	void THREEPPRenderer::OnMousePress(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		wxPoint pos = event.GetPosition();
		int button = 0;
		if (wxMOUSE_BTN_LEFT == buttonFlag) {
			button = 0;
		}
		else if (wxMOUSE_BTN_RIGHT == buttonFlag) {
			button = 1;
		}
		threepp::Vector2 p{ pos.x, pos.y };
		onMousePressedEvent(button, p, PeripheralsEventSource::MouseAction::PRESS);

		event.Skip();
	}
	void THREEPPRenderer::OnMouseRelease(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		wxPoint pos = event.GetPosition();
		int button = 0;
		if (wxMOUSE_BTN_LEFT == buttonFlag) {
			button = 0;
		}
		else if (wxMOUSE_BTN_RIGHT == buttonFlag) {
			button = 1;
		}
		threepp::Vector2 p{ pos.x, pos.y };
		onMousePressedEvent(button, p, PeripheralsEventSource::MouseAction::RELEASE);

		event.Skip();
	}
	void THREEPPRenderer::OnMouseWheel(wxMouseEvent& event)
	{
		int direction = event.GetWheelRotation() / 120;// 1 or -1
		int xoffset = 0;
		int yoffset = direction;
		onMouseWheelEvent({ static_cast<float>(xoffset), static_cast<float>(yoffset) });
		event.Skip();
	}
	threepp::WindowSize THREEPPRenderer::size() const
	{
		return threepp::WindowSize();
	}
}