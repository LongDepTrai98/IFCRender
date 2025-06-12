#ifndef _CONTEXT_RENDERER_HPP_
#define _CONTEXT_RENDERER_HPP_
/*
* THREEPP CONTEXT RENDERER
*/
namespace dragon
{
	class SceneView;
	class ContextRenderer
	{
	public: 
		ContextRenderer(SceneView* canvas);
		virtual void resize(const int& width, const int& height); 
		virtual void update(const float& dtTime); 
		virtual void render();
		virtual ~ContextRenderer();
	private: 
		SceneView* m_Canvas{ nullptr };
	};
}
#endif // !_CONTEXT_RENDERER_HPP_
