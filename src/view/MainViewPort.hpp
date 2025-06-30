#ifndef _MAIN_VIEW_PORT_HPP_
#define _MAIN_VIEW_PORT_HPP_
#include "core/IViewPort.hpp"
#include "core/IRenderer.hpp"
namespace dragon
{
	class IGeometryCache; 
	class MainViewPort : public ViewPort,
		public IRenderer
	{
	public:
		MainViewPort(RenderCanvas* canvas);
		~MainViewPort();
	public:
		void initCamera(threepp::WindowSize& w_size) override;
		void initScene(threepp::WindowSize& w_size) override;
		void setGeometryOffsetCache(std::unique_ptr<IGeometryCache> cache); 
	public:
		void update(const float& dtTime) override;
		void render(threepp::GLRenderer* renderer) override;
		void resize(const int& width, const int& height) override;
	private: 
		std::unique_ptr<IGeometryCache> m_Geometry_Offset_Cache{ nullptr };
	};
}
#endif // !_MAIN_VIEW_PORT_HPP_