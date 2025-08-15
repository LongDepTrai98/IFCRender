#ifndef _MAP_RENDER_CANVAS_HPP_
#define _MAP_RENDER_CANVAS_HPP_
#include "ui/IGLCanvas.hpp"
#include "input/input.hpp"
#include <memory>
#include <functional>
namespace editor
{
	class Win32View; 
	class Win32RenderFrontEnd; 
}
namespace mbgl
{
	class Map; 
}
class ThreeDCustomDrawableStyleLayerHost; 
namespace dragon
{
	class ToolBarData;
	class ItemData; 
	class MapRenderCanvas : public IGLCanvas
	{
	public:
		MapRenderCanvas(wxWindow* parent, 
			const wxGLAttributes& canvasAttrs);
		~MapRenderCanvas();
	public:
		void OnCallbackToolbarCommand(ToolBarData& data) override;
		void OnMenu(wxCommandEvent& event);
	private:
		void initContextMap();
		void initUI();
		void initCallback(); 
		void initMenu();
		void bindFunction();
	private:
		void OnSize(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnMouseMove(wxMouseEvent& event);
		void OnMousePress(wxMouseEvent& event);
		void OnMouseRelease(wxMouseEvent& event);
		void OnMouseWheel(wxMouseEvent& event);
		void OnDoubleClick(wxMouseEvent& event); 
		void OnClickEnableMSAA(wxCommandEvent& command);
		void OnKeyDown(wxKeyEvent& command);
		void OnKeyUp(wxKeyEvent& command);
		void OnInternalIdle() override;
		ThreeDCustomDrawableStyleLayerHost* getCustomDrawableStyleLayerHost();
		/*RENDER*/
	private:
		void enableMultisampling();
		void disableMultisampling();
	public:
		void activeContext();
		//void deactiveContext();
		void swapBuff();
		wxSize getSize();
		void Invalidate();
	public: 
		/*CALLBACK*/
		std::shared_ptr<std::function<void* (int, std::string)>> m_GetData_Item_Callback{ nullptr };
		std::shared_ptr<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>> m_Toggle_Components_Callback{ nullptr };
	private:
		//main context
		std::unique_ptr<editor::Win32View> m_Backend{ nullptr };
		std::unique_ptr<editor::Win32RenderFrontEnd> m_FrontEnd{ nullptr };
		std::unique_ptr<mbgl::Map> m_Map{ nullptr };
		/* MAIN SCENE*/
		//min size
		wxSize m_MinSize{ 640, 480 };
		float  m_dtTime{ 0.0 };
		wxGLAttributes m_DispAttrs{};
		bool m_bIsDirty{ true };
		MouseState m_MouseState{}; 
	};
}
#endif // !_MAP_RENDER_CANVAS_HPP_