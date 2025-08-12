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
	class MainPass;
	class DepthPass;
	class OutLinePass;
	class MainViewPort : public ViewPort,
		public IRenderer
	{
	public:
		/*VIEW PORT DRAW MODE*/
		enum class DrawMode
		{
			DEFAULT,
			DEBUG
		};

	public:
		MainViewPort(IGLCanvas* canvas);
		~MainViewPort();
	public:
		threepp::Scene* getScene() override;
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
		void OnMenuClick(MenuData& data) override;
	public:
		void initPassRenderer();
		void initRayCaster();
		void setFileContext(std::unique_ptr<IFileContext> file_context);
		void resetFileContext();
		void buildBVH(std::vector<float>& vertices,
			std::vector<unsigned int>& indices);
		void buildBVHWithPtr(float* vertices,
			unsigned int* indices,
			size_t& index_size);
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
		std::shared_ptr<threepp::RawShaderMaterial> sobel_material{ nullptr };
		std::shared_ptr<threepp::Mesh> sobel_mesh{ nullptr };
		std::shared_ptr<MainPass> main_renderer_pass{ nullptr };
		std::shared_ptr<DepthPass> depth_renderer_pass{ nullptr };
		std::shared_ptr<OutLinePass> outline_renderer_pass{ nullptr };
	};
}
#endif // !_MAIN_VIEW_PORT_HPP_