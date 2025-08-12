#pragma once 
#ifndef _CONTEXT_GL_LOCK_HPP_
#define _CONTEXT_GL_LOCK_HPP_
#include "wxInclude.hpp" 
namespace dragon
{
	class ContextLock
	{
	public:
		ContextLock(wxGLCanvas* canvas_, wxGLContext* context_);
		~ContextLock() = default;
	public:
		void lock();
		void unlock();
	protected:
		wxGLCanvas* canvas{ nullptr };
		wxGLContext* context{ nullptr };
	};
}
#endif // !_CONTEXT_GL_LOCK_HPP_