#include "NodeApp.hpp"
#include "RendererOpengl.hpp"
#include "platform.h"
extern "C" {
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
}
#include "graph/Editor.hpp"
NodeApplication::NodeApplication(Platform* platform, 
	const char* name) : m_Platform(platform), m_Name(name)
{
	//create renderer 
	m_Renderer = std::make_unique<RendererOpenGL>(); 
}

NodeApplication::~NodeApplication()
{
	m_Renderer->Destroy(); 
	m_Platform->ApplicationStop(); 
	if (m_Context)
	{
		ImGui::DestroyContext(m_Context);
		m_Context = nullptr;
	}
}

bool NodeApplication::Create(int width, int height)
{
	m_Context = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_Context);
	if (!m_Platform)
		return false; 
	if (!m_Renderer->Create(*m_Platform))
		return false; 
	m_IniFilename = m_Name + ".ini";
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = m_IniFilename.c_str(); 
	io.LogFilename = nullptr; 
	ImGui::StyleColorsDark();
	RecreateFontAtlas();
	m_Platform->AcknowledgeFramebufferScaleChanged(); 
	m_Platform->AcknowledgeFramebufferScaleChanged();
	OnStart(); 
	//Frame(); 
	return true;
}

int NodeApplication::Run()
{
	return 0;
}

void NodeApplication::SetTitle(const char* title)
{
}

bool NodeApplication::Close()
{
	return false;
}

void NodeApplication::Quit()
{
}

const std::string& NodeApplication::GetName() const
{
	// TODO: insert return statement here
	return m_Name; 
}

ImFont* NodeApplication::DefaultFont() const
{
	return nullptr;
}

ImFont* NodeApplication::HeaderFont() const
{
	return nullptr;
}

ImTextureID NodeApplication::LoadTexture(const char* path,const char* name)
{
    int width = 0, height = 0, component = 0;
    if (auto data = stbi_load(path, &width, &height, &component, 4))
    {
        auto texture = CreateTexture(data, width, height);
        stbi_image_free(data);
        return texture;
    }
    else
        return nullptr;
}

ImTextureID NodeApplication::GetTextureIDWithName(const char* name)
{
	return ImTextureID();
}

ImTextureID NodeApplication::CreateTexture(const void* data, int width, int height)
{
    return m_Renderer->CreateTexture(data, width, height);
}

void NodeApplication::DestroyTexture(ImTextureID texture)
{
    m_Renderer->DestroyTexture(texture);
}

int NodeApplication::GetTextureWidth(ImTextureID texture)
{
    return m_Renderer->GetTextureWidth(texture);
}

int NodeApplication::GetTextureHeight(ImTextureID texture)
{
    return m_Renderer->GetTextureHeight(texture);
}

void NodeApplication::OnStart()
{
    if (!m_Editor)
    {
        m_Editor = std::make_unique<editor::Editor>(this); 
        m_Editor->createEditorContext("editor");
    }
}

void NodeApplication::OnStop()
{
}

void NodeApplication::OnFrame(float deltaTime)
{
}

ImGuiWindowFlags NodeApplication::GetWindowFlags() const
{
    return
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBringToFrontOnFocus;
}

void NodeApplication::RecreateFontAtlas()
{
    ImGuiIO& io = ImGui::GetIO();
    ImFontConfig config;
    config.OversampleH = 4;
    config.OversampleV = 4;
    config.PixelSnapH = false;
    m_DefaultFont = io.Fonts->AddFontFromFileTTF("D:/GITHUB/editor/GUI_MFC/WFEditor/data/OpenSans-Italic-VariableFont_wdth,wght.ttf", 20.0f, &config, io.Fonts->GetGlyphRangesVietnamese());
    m_HeaderFont = io.Fonts->AddFontFromFileTTF("D:/GITHUB/editor/GUI_MFC/WFEditor/data/OpenSans-VariableFont_wdth,wght.ttf", 25.0f, &config, io.Fonts->GetGlyphRangesVietnamese());
    io.Fonts->Build();
}

void NodeApplication::Frame()
{
    auto& io = ImGui::GetIO();

    if (m_Platform->HasWindowScaleChanged())
        m_Platform->AcknowledgeWindowScaleChanged();

    if (m_Platform->HasFramebufferScaleChanged())
    {
        RecreateFontAtlas();
        m_Platform->AcknowledgeFramebufferScaleChanged();
    }

    const float windowScale = m_Platform->GetWindowScale();
    const float framebufferScale = m_Platform->GetFramebufferScale();

    if (io.WantSetMousePos)
    {
        io.MousePos.x *= windowScale;
        io.MousePos.y *= windowScale;
    }

    m_Platform->NewFrame();

    // Don't touch "uninitialized" mouse position
    if (io.MousePos.x > -FLT_MAX && io.MousePos.y > -FLT_MAX)
    {
        io.MousePos.x /= windowScale;
        io.MousePos.y /= windowScale;
    }
    io.DisplaySize.x /= windowScale;
    io.DisplaySize.y /= windowScale;
    io.DisplayFramebufferScale.x = framebufferScale;
    io.DisplayFramebufferScale.y = framebufferScale;

    m_Renderer->NewFrame();

    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    const auto windowBorderSize = ImGui::GetStyle().WindowBorderSize;
    const auto windowRounding = ImGui::GetStyle().WindowRounding;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("Content", nullptr, GetWindowFlags());
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, windowRounding);

    OnFrame(io.DeltaTime);
    if (m_Platform)
    {
        m_Platform->OnFrame(io.DeltaTime); 
    }
    if (m_Editor)
    {
        m_Editor->editorOnFrame(io.DeltaTime); 
    }
    ImGui::PopStyleVar(2);
    ImGui::End();
    ImGui::PopStyleVar(2);

    // Rendering
    m_Renderer->Clear(ImColor(32, 32, 32, 255));
    ImGui::Render();
    m_Renderer->RenderDrawData(ImGui::GetDrawData());
    m_Platform->FinishFrame();
}

AppRenderer* NodeApplication::getRenderer()
{
    return m_Renderer.get(); 
}
