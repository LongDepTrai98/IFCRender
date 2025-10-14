#include "NodeApp.hpp"

NodeApplication::NodeApplication(const char* name) : m_Name(name)
{
}

NodeApplication::~NodeApplication()
{
}

bool NodeApplication::Create(int width, int height)
{
	return false;
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
	//insert data for map texture
	return ImTextureID();
}

ImTextureID NodeApplication::GetTextureIDWithName(const char* name)
{
	return ImTextureID();
}

ImTextureID NodeApplication::CreateTexture(const void* data, int width, int height)
{
	return ImTextureID();
}

void NodeApplication::DestroyTexture(ImTextureID texture)
{
}

int NodeApplication::GetTextureWidth(ImTextureID texture)
{
	return 0;
}

int NodeApplication::GetTextureHeight(ImTextureID texture)
{
	return 0;
}

ImGuiWindowFlags NodeApplication::GetWindowFlags() const
{
	return ImGuiWindowFlags();
}

void NodeApplication::RecreateFontAtlas()
{
}

void NodeApplication::Frame()
{
}
