#ifndef _MAIN_VIEW_PORT_HPP_
#define _MAIN_VIEW_PORT_HPP_
#include "core/IViewPort.hpp"
#include "core/IRenderer.hpp"
namespace dragon
{
	class MainViewPort : public ViewPort,
		public IRenderer
	{
	public:
		MainViewPort(RenderCanvas* canvas);
		~MainViewPort();
	public:
		void initCamera(threepp::WindowSize& w_size) override;
		void initScene(threepp::WindowSize& w_size) override;
		void addLight(); 
	private: 
		void createExampleScene();
	public:
		void update(const float& dtTime) override;
		void render(threepp::GLRenderer* renderer) override;
		void resize(const int& width, const int& height) override;
	public:
	};
}
#endif // !_MAIN_VIEW_PORT_HPP_