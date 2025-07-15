#include "MainViewPort.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"
#include "core/io/IFCGeometryCache.hpp"
#include "core/io/IFileContext.hpp"
#include "raycast/CustomRayCaster.hpp"
namespace dragon
{
	MainViewPort::MainViewPort(RenderCanvas* canvas) : IRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Viewport_Size = { canvas_size.x, canvas_size.y };
		initScene(m_Viewport_Size);
		initCamera(m_Viewport_Size);
		initRayCaster();
	}
	MainViewPort::~MainViewPort()
	{
	}
	void MainViewPort::initCamera(threepp::WindowSize& w_size)
	{
		if (!m_Camera)
			m_Camera = std::make_unique<threepp::PerspectiveCamera>(60, w_size.aspect(), 0.1f, 2000.f);
		m_Camera->layers.enableAll();
		m_Camera->position.z = 10;
	}
	void MainViewPort::initScene(threepp::WindowSize& w_size)
	{
		if (!m_Scene)
			m_Scene = std::make_unique<threepp::Scene>();
		m_Scene->background = 0x2A2A2A;
	}
	void MainViewPort::initRayCaster()
	{
		if (!m_RayCaster)
			m_RayCaster = std::make_unique<CustomRayCaster>();
	}
	
	void MainViewPort::setFileContext(std::unique_ptr<IFileContext> file_context)
	{
		if (m_FileContext)
		{
			m_FileContext.reset();
			m_FileContext = nullptr;
		}
		m_FileContext = std::move(file_context);
	}
	void MainViewPort::resetFileContext()
	{
		if (!m_FileContext)m_FileContext.reset();
		m_FileContext = nullptr;
	}
	void MainViewPort::buildBVH(std::vector<float>& vertices,
		std::vector<unsigned int>& indices)
	{
		if (m_RayCaster) {
			m_RayCaster->buildBVH(vertices,indices);
		}
	}
	void MainViewPort::clearBVH()
	{
		if (m_RayCaster)
		{
			m_RayCaster->clearBVH();
		}
	}
	void MainViewPort::clearScene()
	{
		if (m_Scene)
		{
			m_Scene->clear();
		}
		clearBVH();
	}
	CustomRayCaster* MainViewPort::getRayCaster()
	{
		return m_RayCaster.get(); 
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
	void MainViewPort::handleRaycast(threepp::Vector2& nor_mouse_pos)
	{
		m_RayCaster->setFromCamera(nor_mouse_pos,
			*m_Camera.get());
		if (m_FileContext)
		{
			m_FileContext->handleRaycast(*m_RayCaster.get(), nor_mouse_pos);
			m_FileContext->handleHoverResult();
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
			renderer->setViewport({ 0,0,m_Viewport_Size.width(),m_Viewport_Size.height() });
			renderer->render(*m_Scene.get(), *m_Camera.get());
		}
	}
}