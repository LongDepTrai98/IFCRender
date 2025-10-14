# pragma once
#ifndef _NODE_APP_HPP_
#define _NODE_APP_HPP_
# include <imgui.h>
# include <string>
# include <memory>
struct Platform;
struct AppRenderer;

class NodeApplication
{
public: 
    NodeApplication(Platform* platform, const char* name);
    ~NodeApplication();
public:
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
    virtual void OnStart(); 
    virtual void OnStop(); 
    virtual void OnFrame(float deltaTime); 
    virtual ImGuiWindowFlags GetWindowFlags() const;
    virtual bool CanClose() { return true; }
    void Frame();
    AppRenderer* getRenderer(); 
private:
    void RecreateFontAtlas();
    std::string                 m_Name;
    std::string                 m_IniFilename;
    Platform*   m_Platform;
    std::unique_ptr<AppRenderer>   m_Renderer;
    ImGuiContext* m_Context = nullptr;
    ImFont* m_DefaultFont = nullptr;
    ImFont* m_HeaderFont = nullptr;
};
#endif // !_NODE_APP_HPP_

