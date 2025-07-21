#include "MainViewPort.hpp"
#include "wxInclude.hpp"
#include "ui/RenderCanvas.hpp"
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"
#include "threepp/renderers/GLRenderTarget.hpp"
#include "threepp/materials/RawShaderMaterial.hpp"
#include "threepp/core/InterleavedBuffer.hpp"
#include "core/io/IFCGeometryCache.hpp"
#include "core/io/IFileContext.hpp"
#include "core/Paths.hpp"
#include "core/utils/StringHelper.hpp"
#include "raycast/CustomRayCaster.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "core/drawpass/OutLinePass.hpp"
#include "core/drawpass/DepthPass.hpp"
#include "threepp/core/InterleavedBufferAttribute.hpp"
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

			if (!sobel_mesh)
			{

				std::vector<float> float32Array{
				-1.f,  1.f, 0.f,  0.f, 1.f, // top-left
				-1.f, -1.f, 0.f,  0.f, 0.f, // bottom-left
				 1.f, -1.f, 0.f,  1.f, 0.f, // bottom-right
				 1.f,  1.f, 0.f,  1.f, 1.f  // top-right
				};
				auto _geometry = threepp::BufferGeometry::create();
				auto interleavedBuffer = threepp::InterleavedBuffer::create(float32Array, 5);
				_geometry->setIndex(std::vector<int>{0, 1, 2, 0, 2, 3});
				_geometry->setAttribute("position", std::make_unique<threepp::InterleavedBufferAttribute>(interleavedBuffer, 3, 0, false));
				_geometry->setAttribute("uv", std::make_unique<threepp::InterleavedBufferAttribute>(interleavedBuffer, 2, 3, false));

				if (!sobel_material)
				{
					this->sobel_material = threepp::RawShaderMaterial::create();
					const std::string vertex_path = assets::Shader + "outline.vert";
					const std::string frag_path = assets::Shader + "outline.frag";
					std::string vertexSource{};
					std::string fragSource{};
					/*vertexSource = StringHelper::ReadFile(vertex_path);
					fragSource = StringHelper::ReadFile(frag_path);*/
					StringHelper::loadBinaryFile(vertex_path, &vertexSource); 
					//StringHelper::loadBinaryFile("D:\\Code\\IFCRender\\src\\shaders\\outline.frag", &fragSource);

					fragSource = R"(
						#version 330 core 
uniform sampler2D depthTex;
uniform vec2 resolution;
uniform float threshold = 0.2; 

in vec2 vUv;
out vec4 FragColor;

void main() {
    vec2 texelSize = 1.0 / resolution;
    
    // Sobel kernels
    float tl = texture(depthTex, vUv + vec2(-texelSize.x, -texelSize.y)).r; // top-left
    float tm = texture(depthTex, vUv + vec2(0.0, -texelSize.y)).r;          // top-middle  
    float tr = texture(depthTex, vUv + vec2(texelSize.x, -texelSize.y)).r;  // top-right
    float ml = texture(depthTex, vUv + vec2(-texelSize.x, 0.0)).r;          // middle-left
    float mm = texture(depthTex, vUv).r;                                    // center
    float mr = texture(depthTex, vUv + vec2(texelSize.x, 0.0)).r;           // middle-right
    float bl = texture(depthTex, vUv + vec2(-texelSize.x, texelSize.y)).r;  // bottom-left
    float bm = texture(depthTex, vUv + vec2(0.0, texelSize.y)).r;           // bottom-middle
    float br = texture(depthTex, vUv + vec2(texelSize.x, texelSize.y)).r;   // bottom-right
    
    // Sobel X kernel: [-1 0 1; -2 0 2; -1 0 1]
    float sobelX = (-1.0 * tl) + (0.0 * tm) + (1.0 * tr) +
                   (-2.0 * ml) + (0.0 * mm) + (2.0 * mr) +
                   (-1.0 * bl) + (0.0 * bm) + (1.0 * br);
    
    // Sobel Y kernel: [-1 -2 -1; 0 0 0; 1 2 1]  
    float sobelY = (-1.0 * tl) + (-2.0 * tm) + (-1.0 * tr) +
                   ( 0.0 * ml) + ( 0.0 * mm) + ( 0.0 * mr) +
                   ( 1.0 * bl) + ( 2.0 * bm) + ( 1.0 * br);
    
    // Calculate edge magnitude
    float edgeMagnitude = sqrt(sobelX * sobelX + sobelY * sobelY);
    
    if (edgeMagnitude > threshold) {
        // have edge
        FragColor = vec4(1.0, 0.552, 0.0, 1.0); 
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0); // Transparent
    }
}
					)"; 

					sobel_material->vertexShader = vertexSource;
					sobel_material->fragmentShader = fragSource;
				}

				sobel_mesh = threepp::Mesh::create(_geometry,sobel_material); 

				if (test_outline_pass)
					test_outline_pass->getScene()->add(sobel_mesh); 
			}

			};

		/*HARD CODE TEST RENDER PASS*/
		if (!test_depth_pass)
		{
			test_depth_pass = std::make_unique<DepthPass>(m_Viewport_Size.width(), m_Viewport_Size.height());
		}

		if (!test_outline_pass)
		{
			test_outline_pass = std::make_unique<OutLinePass>(m_Viewport_Size.width(), m_Viewport_Size.height());
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
				/*MAIN SCENE*/
				renderer->setClearColor(threepp::Color(0x000000), 1);
				renderer->clear();
				renderer->render(*m_Scene.get(), *m_Camera.get());
				/*DRAW DEPTH*/
				//renderer->clear();  
				renderer->setRenderTarget(test_depth_pass->getRenderTarget()); 
				renderer->render(*test_depth_pass->getScene(), *m_Camera.get()); 
				//renderer->render(*m_Scene.get(), *m_Camera.get());
				renderer->setRenderTarget(nullptr);
				/*DRAW OUTLINE*/


				if (sobel_material)
				{
					sobel_material->uniforms["depthTex"].setValue(test_depth_pass->getRenderTarget()->texture.get()); 
					auto size = renderer->size();
					sobel_material->uniforms["resolution"].setValue(threepp::Vector2(size.width(), size.height()));
					sobel_material->transparent = true; 
				}
				sobel_material->depthTest = false; 
				renderer->state().setBlending(threepp::Blending::Normal);
				renderer->render(*test_outline_pass->getScene(), *m_Camera.get());
				renderer->state().setBlending(threepp::Blending::None);
				sobel_material->depthTest = true;
			}
			default:
				break;
			}
		}
	}
}