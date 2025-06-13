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
		virtual void resize(const int& width, const int& height) = 0;
		virtual void update(const float& dtTime) = 0; 
		virtual void render() = 0; 
	public: 
		//event for renderer 
		virtual void OnMouseMove(wxMouseEvent& event) = 0;
		virtual void OnMousePress(wxMouseEvent& event) = 0;
		virtual void OnMouseRelease(wxMouseEvent& event) = 0;
		virtual void OnMouseWheel(wxMouseEvent& event) = 0 ;
	protected: 
		RenderCanvas* m_Canvas{ nullptr };
	};
}
#endif // !_I_CONTEXT_RENDERER_HPP_
