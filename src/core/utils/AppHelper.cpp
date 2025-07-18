#include "AppHelper.hpp"
#include "ui/WindowFrame.hpp"
#include "ui/RenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "threepp/threepp.hpp"
#include "spdlog/spdlog.h"
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

	RenderCanvas* AppHelper::getRenderCanvas(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		return main_frame->getRenderCanvas();
	}

	ElementTreeCtrl* AppHelper::getMainTreeCtrl(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		return main_frame->getElementTreeCtrl();
	}
	wxBitmapBundle AppHelper::loadBitmapBundle(const std::string& path, wxBitmapType type)
	{
		wxIcon icon(path, type);
		wxSize iconSize(icon.GetWidth(), icon.GetHeight());
		spdlog::info("Load Resource : {}", path);
		return wxBitmapBundle::FromImpl(new FixedSizeImpl(iconSize, icon));
	}
}