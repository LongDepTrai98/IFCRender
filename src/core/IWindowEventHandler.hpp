#ifndef _IWINDOW_EVENT_HANDLER_
#define _IWINDOW_EVENT_HANDLER_
#include "wxInclude.hpp"
namespace dragon
{
	class WindowEventHandler
	{
	public:
		virtual void OnMouseMove(wxMouseEvent& event) = 0;
		virtual void OnMousePress(wxMouseEvent& event) = 0;
		virtual void OnMouseRelease(wxMouseEvent& event) = 0;
		virtual void OnMouseWheel(wxMouseEvent& event) = 0;
	};
}
#endif // !_IWINDOW_EVENT_HANDLER_