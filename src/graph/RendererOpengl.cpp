#include "RendererOpengl.hpp"

RendererOpenGL::~RendererOpenGL()
{
}

bool RendererOpenGL::Create(Platform& platform)
{
    m_Platform = &platform;
    // Technically we should initialize OpenGL context here,
   // but for now we relay on one created by GLFW3

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
        return false;

# if PLATFORM(MACOS)
    const char* glslVersion = "#version 150";
# else
    const char* glslVersion = "#version 330";
# endif

    if (!ImGui_ImplOpenGL3_Init(glslVersion))
        return false;

    m_Platform->SetRenderer(this);
    return true;
}

void RendererOpenGL::Destroy()
{
    if (!m_Platform)
        return;
    m_Platform->SetRenderer(nullptr);
    ImGui_ImplOpenGL3_Shutdown();
}

void RendererOpenGL::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
}

void RendererOpenGL::RenderDrawData(ImDrawData* drawData)
{
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
}

void RendererOpenGL::Clear(const ImVec4& color)
{
    glClearColor(color.x, color.y, color.z, color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererOpenGL::Present()
{
}

void RendererOpenGL::Resize(int width, int height)
{
    glViewport(0, 0, width, height);
}

ImVector<ImTexture>::iterator RendererOpenGL::FindTexture(ImTextureID texture)
{
    auto textureID = static_cast<GLuint>(reinterpret_cast<std::intptr_t>(texture));

    return std::find_if(m_Textures.begin(), m_Textures.end(), [textureID](ImTexture& texture)
        {
            return texture.TextureID == textureID;
        });
}

ImTextureID RendererOpenGL::CreateTexture(const void* data, int width, int height)
{
    m_Textures.resize(m_Textures.size() + 1);
    ImTexture& texture = m_Textures.back();

    // Upload texture to graphics system
    GLint last_texture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &texture.TextureID);
    glBindTexture(GL_TEXTURE_2D, texture.TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, last_texture);

    texture.Width = width;
    texture.Height = height;

    return reinterpret_cast<ImTextureID>(static_cast<std::intptr_t>(texture.TextureID));
}

void RendererOpenGL::DestroyTexture(ImTextureID texture)
{
    auto textureIt = FindTexture(texture);
    if (textureIt == m_Textures.end())
        return;
    glDeleteTextures(1, &textureIt->TextureID);
    m_Textures.erase(textureIt);
}

int RendererOpenGL::GetTextureWidth(ImTextureID texture)
{
    auto textureIt = FindTexture(texture);
    if (textureIt != m_Textures.end())
        return textureIt->Width;
    return 0;
}

int RendererOpenGL::GetTextureHeight(ImTextureID texture)
{
    auto textureIt = FindTexture(texture);
    if (textureIt != m_Textures.end())
        return textureIt->Height;
    return 0;
}
