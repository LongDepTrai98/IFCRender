# pragma once
#ifndef _APP_RENDERER_HPP_
#define _APP_RENDERER_HPP_
struct Platform;
struct ImDrawData;
struct ImVec4;
using ImTextureID = void*;
class AppRenderer
{
public: 
    virtual ~AppRenderer() {};

    virtual bool Create(Platform& platform) = 0;
    virtual void Destroy() = 0;

    virtual void NewFrame() = 0;

    virtual void RenderDrawData(ImDrawData* drawData) = 0;

    virtual void Clear(const ImVec4& color) = 0;
    virtual void Present() = 0;

    virtual void Resize(int width, int height) = 0;

    virtual ImTextureID CreateTexture(const void* data, int width, int height) = 0;
    virtual void        DestroyTexture(ImTextureID texture) = 0;
    virtual int         GetTextureWidth(ImTextureID texture) = 0;
    virtual int         GetTextureHeight(ImTextureID texture) = 0;
};
#endif // !_APP_RENDERER_HPP_

