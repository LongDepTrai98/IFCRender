#ifndef _IFILE_CONTEXT_HPP_
#define _IFILE_CONTEXT_HPP_
#include <string>
#include <memory>
#include <functional>
#include "threepp/math/Vector2.hpp"
namespace threepp
{
	class Raycaster;
	class Mesh;
}
namespace dragon
{
	class IGeometryCache;
	class CustomRayCaster;
	class MouseState;
	class EventData;
	class KeyData;
	class ToolBarData;
	class MenuData; 
	class IFileContext
	{
	public:
		virtual ~IFileContext() {};
	public:
		virtual void initLayerOverLay() = 0;
	public:
		virtual std::string getFileType() = 0;
		virtual void handleRaycast(CustomRayCaster& RayCaster, MouseState& mouse_state) = 0;
		virtual void handleHoverResult() = 0;
		virtual void LButtonUp(EventData& data) = 0;
		virtual void LButtonDown(EventData& data) = 0;
		virtual void RButtonUp(EventData& data) = 0;
		virtual void RButtonDown(EventData& data) = 0;
		virtual void KeyDown(KeyData& data) = 0;
		virtual void KeyUp(KeyData& data) = 0;
		virtual void ToolBarAction(ToolBarData& data) = 0;
		virtual void MenuClick(MenuData& data) = 0; 
	public:
		/*DEBUG*/
	public:
		bool m_bIsEnableHover{ true };
		std::function<void()> OnRedrawCallback{ nullptr };
	};
}
#endif // !_IFILE_CONTEXT_HPP_