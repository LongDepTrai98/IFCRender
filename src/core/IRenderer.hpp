#ifndef _I_CONTEXT_RENDERER_HPP_
#define _I_CONTEXT_RENDERER_HPP_
#include "ui/IGLCanvas.hpp"
class wxMouseEvent;
namespace dragon
{
	class RenderCanvas;
	class IRenderer
	{
	public:
		IRenderer(IGLCanvas* canvas) : m_Canvas(canvas) {};
		virtual ~IRenderer() {};
		//public:
		//	/*EVENT RENDER*/
		virtual void update(const float& dtTime) = 0;
	public:
		IGLCanvas* m_Canvas{ nullptr };
	};
}
#endif // !_I_CONTEXT_RENDERER_HPP_