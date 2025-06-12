#ifndef _CONTEXT_RENDERER_HPP_
#define _CONTEXT_RENDERER_HPP_
#include "IContextRenderer.hpp"
#include "threepp/threepp.hpp"
namespace dragon
{
	/*
	* THREEPP BACKEND RENDERER
	*/
	class TPContextRenderer : public IContextRenderer
	{
	public: 
		TPContextRenderer(RenderCanvas* canvas);
	private: 
		void initRenderer(threepp::WindowSize& w_size);
		void initCamera(threepp::WindowSize& w_size);
		void initScene(threepp::WindowSize& w_size); 
		void ctxRender();
	//example 
	private: 
		void createExampleScene();
	public: 
		virtual void resize(const int& width, const int& height); 
		virtual void update(const float& dtTime); 
		virtual void render();
		virtual ~TPContextRenderer();
	private: 
		std::unique_ptr<threepp::GLRenderer> m_Renderer{nullptr}; 
		std::unique_ptr<threepp::Scene> m_Scene{nullptr}; 
		std::unique_ptr<threepp::PerspectiveCamera> m_Camera{nullptr}; 
		//threepp::OrbitControls oritConstrols; 
	};
}
#endif // !_CONTEXT_RENDERER_HPP_
