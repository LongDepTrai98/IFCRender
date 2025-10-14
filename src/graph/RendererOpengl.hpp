#ifndef _RENDERER_OPENGL_HPP_
#define _RENDERER_OPENGL_HPP_
#include <GL/glew.h>
#include "AppRenderer.hpp"
#include <algorithm>
#include <cstdint>
#include "imgui_impl_opengl3.h"
#include "platform.h"
struct ImTexture
{
    GLuint TextureID = 0;
    int    Width = 0;
    int    Height = 0;
};
class RendererOpenGL : public AppRenderer
{
public: 
    ~RendererOpenGL(); 
public: 
    bool Create(Platform &platform) override;
    void Destroy() override;
    void NewFrame() override;
    void RenderDrawData(ImDrawData* drawData) override;
    void Clear(const ImVec4& color) override;
    void Present() override;
    void Resize(int width, int height) override;

    ImVector<ImTexture>::iterator FindTexture(ImTextureID texture);
    ImTextureID CreateTexture(const void* data, int width, int height) override;
    void        DestroyTexture(ImTextureID texture) override;
    int         GetTextureWidth(ImTextureID texture) override;
    int         GetTextureHeight(ImTextureID texture) override;

    Platform* m_Platform = nullptr;
    ImVector<ImTexture>     m_Textures;
};
#endif // !_RENDERER_OPENGL_HPP_
