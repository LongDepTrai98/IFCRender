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
		THREEPPRenderer* three_renderer = dynamic_cast<THREEPPRenderer*>(renderer);
		if (!renderer) return nullptr;
		MainViewPort* viewport = dynamic_cast<MainViewPort*>(three_renderer->getMainViewPort());
		return viewport;
	}
}