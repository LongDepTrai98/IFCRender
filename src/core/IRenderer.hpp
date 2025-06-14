#ifndef _I_CONTEXT_RENDERER_HPP_
#define _I_CONTEXT_RENDERER_HPP_
class wxMouseEvent;
namespace dragon
{
	class RenderCanvas;
	class IRenderer
	{
	public:
		IRenderer(RenderCanvas* canvas) : m_Canvas(canvas) {};
		virtual ~IRenderer() {};
	public:
		/*EVENT RENDER*/
		virtual void resize(const int& width, const int& height) = 0;
		virtual void update(const float& dtTime) = 0;
		virtual void render() = 0;
	protected:
		RenderCanvas* m_Canvas{ nullptr };
	};
}
#endif // !_I_CONTEXT_RENDERER_HPP_