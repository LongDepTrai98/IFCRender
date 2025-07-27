#include "MainViewPort.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
#include "threepp/core/InterleavedBuffer.hpp"
#include "core/io/IFCGeometryCache.hpp"
#include "core/io/IFileContext.hpp"
#include "core/Paths.hpp"
#include "raycast/CustomRayCaster.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "core/drawpass/MainPass.hpp"
#include "core/drawpass/OutLinePass.hpp"
#include "core/drawpass/DepthPass.hpp"
#include "config/app_config.hpp"
#include "spdlog/spdlog.h"

namespace dragon
{
	MainViewPort::MainViewPort(RenderCanvas* canvas) : IRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Viewport_Size = { canvas_size.x, canvas_size.y };
		initScene(m_Viewport_Size);
		initCamera(m_Viewport_Size);
		initPassRenderer();
		initRayCaster();
		m_Add_Object_CallBack = [&](const std::vector<std::shared_ptr<threepp::Mesh>>& meshes) {
			for (auto& mesh : meshes)
			{
				m_Scene->add(mesh);
			}
			};
		m_Add_Object_DrawDepth_CallBack = [&](const std::vector<std::shared_ptr<threepp::BufferGeometry>>& geometries, bool isSelectMultiMesh) {
			if (depth_renderer_pass)
			{
				if (!isSelectMultiMesh)
					depth_renderer_pass->updateOneDepthMeshSelect(geometries[0]);
				else
					depth_renderer_pass->updateMultiDepthMeshSelect(geometries[0]);
			}
			};
	}
	MainViewPort::~MainViewPort()
	{
	}
	threepp::Scene* MainViewPort::getScene()
	{
		if (!main_renderer_pass) return nullptr;
		return main_renderer_pass->getScene();
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
	}
	void MainViewPort::OnLButtonDown(EventData& data)
	{
		data.camera = m_Camera.get();
		if (m_FileContext)
			m_FileContext->LButtonDown(data);
	}
	void MainViewPort::OnLButtonUp(EventData& data)
	{
		data.camera = m_Camera.get();
		if (m_FileContext)
			m_FileContext->LButtonUp(data);
	}
	void MainViewPort::OnRButtonUp(EventData& data)
	{
		data.camera = m_Camera.get();
		if (m_FileContext)
			m_FileContext->RButtonUp(data);
	}
	void MainViewPort::OnKeyDown(KeyData& data)
	{
		if (m_FileContext)
			m_FileContext->KeyDown(data);
	}
	void MainViewPort::OnKeyUp(KeyData& data)
	{
		if (m_FileContext)
			m_FileContext->KeyUp(data);
	}
	void MainViewPort::OnToolActions(int toolID)
	{
	}
	void MainViewPort::OnToolBarAction(ToolBarData& data)
	{
		if (m_FileContext)
			m_FileContext->ToolBarAction(data);
		if (data.event.GetId() == (int)ID_EVENT::TOOL_SWITCH_MODE_RENDER)
		{
			/*SWITCH CURRENT MODE*/
			data.bIsCheck ? m_Current_Draw_Mode = DrawMode::DEBUG : m_Current_Draw_Mode = DrawMode::DEFAULT;
		}
		if (data.event.GetId() == (int)ID_EVENT::CREATE_NEW_RENDER_TARGET)
		{
			//depth_renderer_pass->reCreateRenderTarget(m_Viewport_Size.width(), m_Viewport_Size.height());
		}
		if (data.event.GetId() == (int)ID_EVENT::TOOL_DRAW_GRID)
		{
			if (main_renderer_pass)
			{
				auto grid_mesh = main_renderer_pass->getScene()->getObjectByName("Scene_Grid");
				if (grid_mesh)
				{
					grid_mesh->visible = !grid_mesh->visible;
				}
			}
		}
		if (data.event.GetId() == (int)ID_EVENT::TOOL_CLEAR_SCENE)
		{
			if (main_renderer_pass)
				main_renderer_pass->clear();
			m_FileContext.reset();
			m_FileContext = nullptr;
		}
		m_Canvas->Invalidate();
	}
	void MainViewPort::OnMenuClick(MenuData& data)
	{
		if (m_FileContext)
			m_FileContext->MenuClick(data);
	}
	void MainViewPort::OnRButtonDown(EventData& data)
	{
		data.camera = m_Camera.get();
		if (m_FileContext)
			m_FileContext->RButtonDown(data);
	}
	void MainViewPort::initPassRenderer()
	{
		if (!main_renderer_pass)
			main_renderer_pass = std::make_shared<MainPass>(m_Viewport_Size.width(), m_Viewport_Size.height());
		if (!depth_renderer_pass)
		{
			depth_renderer_pass = std::make_shared<DepthPass>(m_Viewport_Size.width(), m_Viewport_Size.height());
		}
		if (!outline_renderer_pass)
			outline_renderer_pass = std::make_shared<OutLinePass>(m_Viewport_Size.width(), m_Viewport_Size.height());
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
			m_RayCaster->buildBVH(vertices, indices);
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
		if (main_renderer_pass)
			main_renderer_pass->clear();
		if (depth_renderer_pass)
			depth_renderer_pass->clear();
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
		glViewport(0, 0, width, height);
		if (depth_renderer_pass) depth_renderer_pass->reCreateRenderTarget(width, height);
	}
	void MainViewPort::handleRaycast(MouseState& mouse_state)
	{
		m_RayCaster->setFromCamera(mouse_state.nor_mouse_pos,
			*m_Camera.get());
		if (m_FileContext)
		{
			m_FileContext->handleRaycast(*m_RayCaster.get(), mouse_state);
			m_FileContext->handleHoverResult();
		}
	}
	void MainViewPort::update(const float& dtTime)
	{
	}
	void MainViewPort::render(threepp::GLRenderer* renderer)
	{
		renderer->setViewport({ 0,0,m_Viewport_Size.width(),m_Viewport_Size.height() });
		if (m_Camera)
		{
			switch (m_Current_Draw_Mode)
			{
			case DrawMode::DEFAULT:
			{
				renderer->setClearColor(default_color::clear_color, 1);
				renderer->clear();
				if (main_renderer_pass)
				{
					main_renderer_pass->render(renderer, m_Camera.get());
				}
				if (depth_renderer_pass)
				{
					depth_renderer_pass->applyUniform(m_Camera->nearPlane, m_Camera->farPlane);
					depth_renderer_pass->render(renderer, m_Camera.get());
				}
				if (outline_renderer_pass)
				{
					outline_renderer_pass->applyUniform(depth_renderer_pass->getRenderTarget()->texture.get());
					outline_renderer_pass->render(renderer, m_Camera.get());
				}
				break;
			}
			case DrawMode::DEBUG:
			{
				if (depth_renderer_pass)
				{
					depth_renderer_pass->applyUniform(m_Camera->nearPlane, m_Camera->farPlane);
					depth_renderer_pass->debugRender(renderer, m_Camera.get());
				}
			}
			default:
				break;
			}
		}
	}
}