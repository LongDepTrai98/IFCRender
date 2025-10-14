#pragma once
#ifndef _DX11_WINDOW_DATA_HPP_
#define _DX11_WINDOW_DATA_HPP_
#include <Windows.h>
#include <imgui.h>
namespace graph
{
	//window data for dx11 
	struct DX11_Window_Data
	{
		int				g_Width;
		int				g_Height;
		bool			m_WindowScaleChanged = false;
		float			m_WindowScale = 1.0f;
		bool			m_FramebufferScaleChanged = false;
		float			m_FramebufferScale = 1.0f;
		ImGuiIO			io;
		ImGuiContext* m_Context = nullptr;
	};
	//window data for ogl 
	struct OGL_Window_Data
	{
		HGLRC			m_Hglrc;
		HWND			m_DummyHWND;
		HDC				m_Hdc; 
		int				g_Width;
		int				g_Height;
		ImGuiIO			io;
		ImGuiContext*	m_Context = nullptr;
	};
}
#endif // !_DX11_WINDOW_DATA_HPP_
