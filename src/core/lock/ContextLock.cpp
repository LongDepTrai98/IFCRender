#include "ContextLock.hpp"

namespace dragon
{
	ContextLock::ContextLock(wxGLCanvas* canvas_, wxGLContext* context_) : 
		canvas(canvas_),context(context_)
	{
	}
	void ContextLock::lock()
	{
		canvas->SetCurrent(*context); 
	}
	void ContextLock::unlock()
	{
		wglMakeCurrent(NULL, NULL); 
	}
}