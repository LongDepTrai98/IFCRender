# pragma once
#ifndef _NODE_APP_HPP_
#define _NODE_APP_HPP_
# include <imgui.h>
# include <string>
# include <memory>
struct Platform;
struct Renderer;

struct NodeApplication
{
    NodeApplication(const char* name);
    ~NodeApplication();

    bool Create(int width = -1, int height = -1);

    int Run();

    void SetTitle(const char* title);

    bool Close();
    void Quit();

    const std::string& GetName() const;

    ImFont* DefaultFont() const;
    ImFont* HeaderFont() const;

    ImTextureID LoadTexture(const char* path,const char* name);
    ImTextureID GetTextureIDWithName(const char* name); 
    ImTextureID CreateTexture(const void* data, int width, int height);
    void        DestroyTexture(ImTextureID texture);
    int         GetTextureWidth(ImTextureID texture);
    int         GetTextureHeight(ImTextureID texture);

    virtual void OnStart() = 0; 
    virtual void OnStop() = 0; 
    virtual void OnFrame(float deltaTime) = 0; 

    virtual ImGuiWindowFlags GetWindowFlags() const;

    virtual bool CanClose() { return true; }

private:
    void RecreateFontAtlas();

    void Frame();

    std::string                 m_Name;
    std::string                 m_IniFilename;
    std::unique_ptr<Platform>   m_Platform;
    std::unique_ptr<Renderer>   m_Renderer;
    ImGuiContext* m_Context = nullptr;
    ImFont* m_DefaultFont = nullptr;
    ImFont* m_HeaderFont = nullptr;
};
#endif // !_NODE_APP_HPP_

