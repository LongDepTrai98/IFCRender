#ifndef _MAIN_VIEW_PORT_HPP_
#define _MAIN_VIEW_PORT_HPP_
#include "core/IViewPort.hpp"
#include "core/IRenderer.hpp"
namespace threepp
{
	class Raycaster; 
}
namespace dragon
{
	class IFileContext; 
	class MainViewPort : public ViewPort,
		public IRenderer
	{
	public:
		MainViewPort(RenderCanvas* canvas);
		~MainViewPort();
	public:
		void initCamera(threepp::WindowSize& w_size) override;
		void initScene(threepp::WindowSize& w_size) override;
		void initRayCaster(); 
		void setFileContext(std::unique_ptr<IFileContext> file_context); 
	public:
		void update(const float& dtTime) override;
		void render(threepp::GLRenderer* renderer) override;
		void resize(const int& width, const int& height) override;
		void handleRaycast(const double& norX, const double& norY) override; 
	private: 
		std::unique_ptr<threepp::Raycaster> m_RayCaster{ nullptr };
		std::unique_ptr<IFileContext> m_FileContext{ nullptr };
	};
}
#endif // !_MAIN_VIEW_PORT_HPP_