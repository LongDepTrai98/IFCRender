#include "MainViewPort.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
#include "threepp/materials/RawShaderMaterial.hpp"
#include "core/io/IFCGeometryCache.hpp"
#include "core/io/IFileContext.hpp"
#include "core/Paths.hpp"
#include "core/utils/StringHelper.hpp"
#include "raycast/CustomRayCaster.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "core/drawpass/OutLinePass.hpp"
#include "core/drawpass/DepthPass.hpp"
namespace dragon
{
	MainViewPort::MainViewPort(RenderCanvas* canvas) : IRenderer(canvas)
	{
		wxSize canvas_size = m_Canvas->getSize();
		m_Viewport_Size = { canvas_size.x, canvas_size.y };
		initScene(m_Viewport_Size);
		initCamera(m_Viewport_Size);
		initRayCaster();
		m_Add_Object_CallBack = [&](const std::vector<std::shared_ptr<threepp::Mesh>>& meshes) {
			for (auto& mesh : meshes)
			{
				m_Scene->add(mesh);
			}
			};
		m_Add_Object_DrawDepth_CallBack = [&](const std::vector<std::shared_ptr<threepp::Mesh>>& meshes) {
			if (!this->depth_material)
			{
				this->depth_material = threepp::RawShaderMaterial::create();
				const std::string vertex_path = assets::Shader + "depth.vert";
				const std::string frag_path = assets::Shader + "depth.frag";
				std::string vertexSource{};
				std::string fragSource{};
				vertexSource = StringHelper::ReadFile(vertex_path);
				fragSource = StringHelper::ReadFile(frag_path);
				depth_material->vertexShader = vertexSource;
				depth_material->fragmentShader = fragSource;
				depth_material->uniforms["near"].setValue(m_Camera->nearPlane);
				depth_material->uniforms["far"].setValue(m_Camera->farPlane);
			}
			for (auto& mesh : meshes)
			{
				std::shared_ptr<threepp::Mesh> meshCopy = threepp::Mesh::create();
				meshCopy->copy(*mesh->as<threepp::Object3D>());
				meshCopy->setMaterial(depth_material);
				test_depth_pass->getScene()->add(meshCopy);
			}
			};

		/*HARD CODE TEST RENDER PASS*/
		if (!test_depth_pass)
		{
			test_depth_pass = std::make_unique<DepthPass>(m_Viewport_Size.width(), m_Viewport_Size.height());
		}

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
			data.bIsCheck ? m_Current_Draw_Mode = DrawMode::DEPTH : m_Current_Draw_Mode = DrawMode::DEFAULT;
		}
		m_Canvas->Invalidate();
	}
	void MainViewPort::OnRButtonDown(EventData& data)
	{
		data.camera = m_Camera.get();
		if (m_FileContext)
			m_FileContext->RButtonDown(data);
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
		if (test_depth_pass)
			test_depth_pass->setSize(width, height); 
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
		if (m_Renderer
			&& m_Scene
			&& m_Camera)
		{
			renderer->setViewport({ 0,0,m_Viewport_Size.width(),m_Viewport_Size.height() });
			switch (m_Current_Draw_Mode)
			{
			case DrawMode::DEFAULT:
			{
				renderer->clear();
				renderer->render(*m_Scene.get(), *m_Camera.get());
				break;
			}
			case DrawMode::DEPTH:
			{
				/*DRAW DEPTH*/
				renderer->clear();  
				renderer->setRenderTarget(test_depth_pass->getRenderTarget()); 
				renderer->render(*test_depth_pass->getScene(), *m_Camera.get()); 
				renderer->setRenderTarget(nullptr);
			}
			default:
				break;
			}
		}
	}
}