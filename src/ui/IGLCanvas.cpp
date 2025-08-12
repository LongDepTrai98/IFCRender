#include "IGLCanvas.hpp"
#include "core/lock/ContextLock.hpp"
namespace dragon
{
	IGLCanvas::IGLCanvas(wxWindow* parent, const wxGLAttributes& canvasAttrs)
		: wxGLCanvas(parent,
			canvasAttrs)
	{
	}
	void IGLCanvas::initGLContext()
	{
		//Create Context
		if (!m_Context)
			m_Context = std::make_unique<wxGLContext>(this, nullptr, &ctxAttrs);
		if (!m_Context->IsOK())
		{
			throw std::exception("Can't create context renderer");
		}
		//Create lock 
		if (!m_ContextLock)
		{
			m_ContextLock = std::make_unique<ContextLock>(this, m_Context.get()); 
		}
	}
	wxGLContext* IGLCanvas::getRenderContext()
	{
		return m_Context.get(); 
	}
	ContextLock* IGLCanvas::getContextLock()
	{
		return m_ContextLock.get(); 
	}
}

