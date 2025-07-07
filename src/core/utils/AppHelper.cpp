#include "AppHelper.hpp"
#include "ui/WindowFrame.hpp"
#include "ui/RenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "threepp/threepp.hpp"

namespace dragon
{
	MainViewPort* AppHelper::getMainViewPortScene(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		IRenderer* renderer = main_frame->getRenderCanvas()->getRenderer();
		THREEPPRenderer* three_renderer = static_cast<THREEPPRenderer*>(renderer);
		if (!renderer) return nullptr;
		MainViewPort* viewport = static_cast<MainViewPort*>(three_renderer->getMainViewPort());
		return viewport;
	}
	ElementTreeCtrl* AppHelper::getMainTreeCtrl(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr; 
		return main_frame->getElementTreeCtrl();
	}
}