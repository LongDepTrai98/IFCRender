#include "THREEPPRenderer.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/threepp.hpp"
#include "threepp/core/Raycaster.hpp"
#include "view/MainViewPort.hpp"
#include "view/ViewportGizmo.hpp"
#include "config/app_config.hpp"
#include <format>
/*
* THREEPP CONTEXT RENDERER
* BACKEND RENDERER IS OPENGL + THREEPP
*/
namespace dragon
{
	THREEPPRenderer::THREEPPRenderer(RenderCanvas* canvas) : IRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Canvas->activeContext();
		//create windows size
		threepp::WindowSize window_size(canvas_size.x, canvas_size.y);
		initRenderer(window_size);
		initViewPort();
		initController();
		m_OrbitControls->update();
		m_Canvas->deactiveContext();
	}
	THREEPPRenderer::~THREEPPRenderer()
	{
	}
	threepp::GLRenderer* THREEPPRenderer::getRenderer()
	{
		return m_Renderer.get();
	}
	ViewPort* THREEPPRenderer::getMainViewPort()
	{
		return m_lstViewPort[index::INDEX_MAIN_VIEWPORT].get();
	}
	void THREEPPRenderer::initRenderer(threepp::WindowSize& w_size)
	{
		if (!m_Renderer)
			m_Renderer = std::make_unique<threepp::GLRenderer>(w_size);
		m_Renderer->autoClear = false;
		/*m_Renderer->shadowMap().enabled = true;
		m_Renderer->shadowMap().type = threepp::ShadowMap::PFC;*/
	}
	void THREEPPRenderer::initViewPort()
	{
		m_lstViewPort.push_back(std::make_unique<MainViewPort>(m_Canvas));
		m_lstViewPort.push_back(std::make_unique<ViewPortGizmo>(m_Canvas));
		for (auto& view_port : m_lstViewPort)
		{
			view_port->initRenderer(m_Renderer.get());
		}
	}
	void THREEPPRenderer::validateContext()
	{
		if (!wglGetCurrentContext())
		{
			throw std::exception("GL context null");
		}
	}
	void THREEPPRenderer::initController()
	{
		if (!m_OrbitControls)
		{
			ViewPort* main_viewport = m_lstViewPort[index::INDEX_MAIN_VIEWPORT].get();
			if (main_viewport)
			{
				m_OrbitControls = std::make_unique<threepp::OrbitControls>(*main_viewport->getCamera(), *this);
				m_OrbitControls->target.set(0.0f, 0.0f, 0.0f);
			}
		}
	}
	void THREEPPRenderer::ctxRender()
	{
		m_Renderer->clear();
		UpdateGizmoFromCamera();
		for (auto& viewport : m_lstViewPort)
		{
			viewport->handleRaycast(m_MouseState);
			viewport->render(m_Renderer.get());
		}
	}
	void THREEPPRenderer::resize(const int& width, const int& height)
	{
		threepp::WindowSize window_size(width, height);
		for (auto& viewport : m_lstViewPort)
		{
			viewport->resize(width, height);
		}
		if (m_Renderer)
		{
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
			/*context render here */
			validateContext();
			ctxRender();
			//m_OrbitControls->update(); 
		}
	}
	void THREEPPRenderer::OnMouseMove(wxMouseEvent& event)
	{
		wxPoint pos = event.GetPosition();
		threepp::Vector2 mousePos(static_cast<float>(pos.x), static_cast<float>(pos.y));
		onMouseMoveEvent(mousePos);
		const wxSize size = m_Canvas->getSize();
		m_MouseState.nor_mouse_pos.x = (pos.x / static_cast<float>(size.GetWidth())) * 2 - 1;
		m_MouseState.nor_mouse_pos.y = -(pos.y / static_cast<float>(size.GetHeight())) * 2 + 1;
	}
	void THREEPPRenderer::OnMousePress(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		wxPoint pos = event.GetPosition();
		threepp::Vector2 p{ pos.x, pos.y };
		int button = 0;
		EventData data;
		data.control = m_OrbitControls.get();
		data.p = p;
		if (wxMOUSE_BTN_LEFT == buttonFlag) {
			for (auto& viewport : m_lstViewPort)
			{
				viewport->OnLButtonDown(data);
			}
			m_MouseState.isLButtonDown = true;
			button = 0;
		}
		else if (wxMOUSE_BTN_RIGHT == buttonFlag) {
			for (auto& viewport : m_lstViewPort)
			{
				viewport->OnRButtonDown(data);
			}
			m_MouseState.isRButtonDown = true;
			button = 1;
		}
		onMousePressedEvent(button, p, PeripheralsEventSource::MouseAction::PRESS);
	}
	void THREEPPRenderer::OnMouseRelease(wxMouseEvent& event)
	{
		int buttonFlag = event.GetButton();
		wxPoint pos = event.GetPosition();
		threepp::Vector2 p{ pos.x, pos.y };
		EventData data;
		data.control = m_OrbitControls.get();
		data.p = p;
		int button = 0;
		if (wxMOUSE_BTN_LEFT == buttonFlag) {
			for (auto& viewport : m_lstViewPort)
			{
				viewport->OnLButtonUp(data);
			}
			m_MouseState.isLButtonDown = false;
			button = 0;
		}
		else if (wxMOUSE_BTN_RIGHT == buttonFlag) {
			for (auto& viewport : m_lstViewPort)
			{
				viewport->OnRButtonUp(data);
			}
			m_MouseState.isRButtonDown = false;
			button = 1;
		}
		onMousePressedEvent(button, p, PeripheralsEventSource::MouseAction::RELEASE);
	}
	void THREEPPRenderer::OnMouseWheel(wxMouseEvent& event)
	{
		int direction = event.GetWheelRotation() / 120;// 1 or -1
		int xoffset = 0;
		int yoffset = direction;
		onMouseWheelEvent({ static_cast<float>(xoffset), static_cast<float>(yoffset) });
	}
	void THREEPPRenderer::OnKeyDown(wxKeyEvent& event)
	{
		KeyData data;
		data.key = event; 
		for (auto& view : m_lstViewPort)
		{
			view->OnKeyDown(data); 
		}
	}
	void THREEPPRenderer::OnKeyUp(wxKeyEvent& event)
	{
		KeyData data;
		data.key = event;
		for (auto& view : m_lstViewPort)
		{
			view->OnKeyUp(data); 
		}
	}
	void THREEPPRenderer::UpdateGizmoFromCamera()
	{
		auto q_main_camera = m_lstViewPort[index::INDEX_MAIN_VIEWPORT]->getCamera()->quaternion;
		auto cube_gizmo = m_lstViewPort[index::INDEX_GIZMO_VIEWPORT]->getScene()->children[0];
		//cube_gizmo->position.set(0.0f, 0.0f, 0.0f);
		cube_gizmo->quaternion.copy(q_main_camera).invert();
		/*cube_gizmo->updateMatrix();
		cube_gizmo->updateMatrixWorld(true);*/
	}
	threepp::WindowSize THREEPPRenderer::size() const
	{
		if (m_Canvas)
		{
			wxSize size = m_Canvas->getSize();
			const int& width = size.x;
			const int& height = size.y;
			return threepp::WindowSize(width, height);
		}
		return threepp::WindowSize();
	}
}