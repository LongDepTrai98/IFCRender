#ifndef _CONTEXT_RENDERER_HPP_
#define _CONTEXT_RENDERER_HPP_
#include "IContextRenderer.hpp"
namespace dragon
{
	/*
	* THREEPP BACKEND RENDERER
	*/
	class TPContextRenderer : public IContextRenderer
	{
	public: 
		TPContextRenderer(SceneView* canvas);
		virtual void resize(const int& width, const int& height); 
		virtual void update(const float& dtTime); 
		virtual void render();
		virtual ~TPContextRenderer();
	private: 
		
	};
}
#endif // !_CONTEXT_RENDERER_HPP_
