#ifndef _IWINDOW_EVENT_HANDLER_
#define _IWINDOW_EVENT_HANDLER_
#include "wxInclude.hpp"
namespace dragon
{
	class ToolBarData;
	class MenuData; 
	class WindowEventHandler
	{
	public:
		virtual void OnMouseMove(wxMouseEvent& event) = 0;
		virtual void OnMousePress(wxMouseEvent& event) = 0;
		virtual void OnMouseRelease(wxMouseEvent& event) = 0;
		virtual void OnMouseWheel(wxMouseEvent& event) = 0;
		virtual void OnKeyDown(wxKeyEvent& event) = 0;
		virtual void OnKeyUp(wxKeyEvent& event) = 0;
		virtual void OnToolBarClick(ToolBarData& data) = 0;
		virtual void OnMenuClick(MenuData& data) = 0; 
	public:
		bool isMouseDown{ false };
	};
}
#endif // !_IWINDOW_EVENT_HANDLER_