#pragma once 
#ifndef _INTERFACE_GL_CANVAS_HPP_
#define _INTERFACE_GL_CANVAS_HPP_
#include "wxInclude.hpp"
#include <memory>
#include <string>
namespace dragon
{
	class ContextLock;
	class ToolBarData;
	class IGLCanvas : public wxGLCanvas
	{
	public: 
		IGLCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs); 
		virtual ~IGLCanvas() = default; 
	public: 
		void initGLContext();
		wxGLContext* getRenderContext(); 
		ContextLock* getContextLock(); 
		const std::string getCanvasName(); 
	public: 
		virtual wxSize getSize() = 0; 
		virtual void Invalidate() = 0;
		virtual void OnCallbackToolbarCommand(ToolBarData& data) = 0; 
	protected: 
		std::unique_ptr<wxGLContext> m_Context{ nullptr }; 
		std::unique_ptr<ContextLock> m_ContextLock{ nullptr };
		std::string m_Name{""};
		wxGLContextAttrs ctxAttrs; 
	};
}
#endif // !_INTERFACE_GL_CANVAS_HPP_
