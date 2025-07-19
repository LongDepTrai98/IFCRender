#ifndef _MAIN_VIEW_PORT_HPP_
#define _MAIN_VIEW_PORT_HPP_
#include "core/IViewPort.hpp"
#include "core/IRenderer.hpp"
namespace threepp
{
	class Raycaster;
	class RawShaderMaterial;
}
namespace dragon
{
	class IFileContext;
	class CustomRayCaster;
	class MainViewPort : public ViewPort,
		public IRenderer
	{
	public:
		/*VIEW PORT DRAW MODE*/
		enum class DrawMode
		{
			DEFAULT,
			DEPTH
		};

	public:
		MainViewPort(RenderCanvas* canvas);
		~MainViewPort();
	public:
		void initCamera(threepp::WindowSize& w_size) override;
		void initScene(threepp::WindowSize& w_size) override;
		void OnLButtonDown(EventData& data) override;
		void OnLButtonUp(EventData& data) override;
		void OnRButtonDown(EventData& data) override;
		void OnRButtonUp(EventData& data) override;
		void OnKeyDown(KeyData& event) override;
		void OnKeyUp(KeyData& event) override;
		void OnToolActions(int toolID) override;
		void OnToolBarAction(ToolBarData& data) override;
	public:
		void initRayCaster();
		void setFileContext(std::unique_ptr<IFileContext> file_context);
		void resetFileContext();
		void buildBVH(std::vector<float>& vertices,
			std::vector<unsigned int>& indices);
		void clearBVH();
		void clearScene() override;
		CustomRayCaster* getRayCaster();
	public:
		void update(const float& dtTime) override;
		void render(threepp::GLRenderer* renderer) override;
		void resize(const int& width, const int& height) override;
		void handleRaycast(MouseState& mouse_state) override;
	private:
		std::unique_ptr<CustomRayCaster> m_RayCaster{ nullptr };
		std::unique_ptr<IFileContext> m_FileContext{ nullptr };
		DrawMode m_Current_Draw_Mode{ DrawMode::DEFAULT };
		/*DEBUG*/
		std::shared_ptr<threepp::RawShaderMaterial> depth_material{ nullptr };
	};
}
#endif // !_MAIN_VIEW_PORT_HPP_