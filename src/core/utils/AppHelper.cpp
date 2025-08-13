#include "AppHelper.hpp"
#include "ui/WindowFrame.hpp"
#include "ui/BimRenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "threepp/threepp.hpp"
#include "spdlog/spdlog.h"
namespace dragon
{
	MainViewPort* AppHelper::getMainBimViewPortScene(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		IGLCanvas* bim_canvas = main_frame->getCanvasWithName("bim-canvas");
		IRenderer* renderer = static_cast<BimRenderCanvas*>(bim_canvas)->getRenderer();
		THREEPPRenderer* three_renderer = static_cast<THREEPPRenderer*>(renderer);
		if (!renderer) return nullptr;
		MainViewPort* viewport = static_cast<MainViewPort*>(three_renderer->getMainViewPort());
		return viewport;
	}

	BimRenderCanvas* AppHelper::getBimRenderCanvas(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		return static_cast<BimRenderCanvas*>(main_frame->getCanvasWithName("bim-canvas"));
	}

	ElementTreeCtrl* AppHelper::getMainTreeCtrl(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		return main_frame->getElementTreeCtrl();
	}
	wxBitmapBundle AppHelper::loadBitmapBundle(const std::string& path, wxBitmapType type)
	{
		wxBitmap bmp(path, wxBITMAP_TYPE_ICO);
		wxImage img = bmp.ConvertToImage();
		img = img.Rescale(13, 13, wxIMAGE_QUALITY_HIGH);
		wxIcon smallIcon;
		smallIcon.CopyFromBitmap(wxBitmap(img));
		wxSize iconSize(smallIcon.GetWidth(), smallIcon.GetHeight());
		spdlog::info("Load Resource : {}", path);
		return wxBitmapBundle::FromImpl(new FixedSizeImpl(iconSize, smallIcon));
	}
	void AppHelper::GetWindowSize(wxWindow* window, int& width, int& height)
	{
		wxSize size = window->GetClientSize();
		width = size.GetWidth(); 
		height = size.GetHeight(); 
	}
	void AppHelper::GetFramebufferSize(wxWindow* window, int* width, int* height)
	{
		wxSize size = window->GetClientSize();
		*width = size.GetWidth();
		*height = size.GetHeight();
	}
}