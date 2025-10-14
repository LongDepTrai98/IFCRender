#include "RendererOpengl.hpp"

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
    return false;
}

void RendererOpenGL::Destroy()
{
}

void RendererOpenGL::NewFrame()
{
}

void RendererOpenGL::RenderDrawData(ImDrawData* drawData)
{
}

void RendererOpenGL::Clear(const ImVec4& color)
{
}

void RendererOpenGL::Present()
{
}

void RendererOpenGL::Resize(int width, int height)
{
}

ImVector<ImTexture>::iterator RendererOpenGL::FindTexture(ImTextureID texture)
{
    return ImVector<ImTexture>::iterator();
}

ImTextureID RendererOpenGL::CreateTexture(const void* data, int width, int height)
{
    return ImTextureID();
}

void RendererOpenGL::DestroyTexture(ImTextureID texture)
{
}

int RendererOpenGL::GetTextureWidth(ImTextureID texture)
{
    return 0;
}

int RendererOpenGL::GetTextureHeight(ImTextureID texture)
{
    return 0;
}
