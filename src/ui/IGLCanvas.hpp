#pragma once 
#ifndef _INTERFACE_GL_CANVAS_HPP_
#define _INTERFACE_GL_CANVAS_HPP_
#include "wxInclude.hpp"
#include <memory>
namespace dragon
{
	class ContextLock;
	class IGLCanvas : public wxGLCanvas
	{
	public: 
		IGLCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs); 
		virtual ~IGLCanvas() = default; 
	public: 
		void initGLContext();
		wxGLContext* getRenderContext(); 
		ContextLock* getContextLock(); 
	public: 
		virtual wxSize getSize() = 0; 
		virtual void Invalidate() = 0;
	protected: 
		std::unique_ptr<wxGLContext> m_Context{ nullptr }; 
		std::unique_ptr<ContextLock> m_ContextLock{ nullptr }; 
		wxGLContextAttrs ctxAttrs; 
	};
}
#endif // !_INTERFACE_GL_CANVAS_HPP_
