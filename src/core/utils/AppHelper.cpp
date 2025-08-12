#include "AppHelper.hpp"
#include "ui/WindowFrame.hpp"
#include "ui/BimRenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "threepp/threepp.hpp"
#include "spdlog/spdlog.h"
namespace dragon
{
	MainViewPort* AppHelper::getMainViewPortScene(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		IRenderer* renderer = main_frame->getBimRenderCanvas()->getRenderer();
		THREEPPRenderer* three_renderer = static_cast<THREEPPRenderer*>(renderer);
		if (!renderer) return nullptr;
		MainViewPort* viewport = static_cast<MainViewPort*>(three_renderer->getMainViewPort());
		return viewport;
	}

	BimRenderCanvas* AppHelper::getRenderCanvas(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		return main_frame->getBimRenderCanvas();
	}

	ElementTreeCtrl* AppHelper::getMainTreeCtrl(WindowFrame* main_frame)
	{
		if (!main_frame) return nullptr;
		return main_frame->getElementTreeCtrl();
	}
	wxBitmapBundle AppHelper::loadBitmapBundle(const std::string& path, wxBitmapType type)
	{
		//wxIcon icon(path, type, 16,16);
		wxBitmap bmp(path, wxBITMAP_TYPE_ICO);
		wxImage img = bmp.ConvertToImage();
		img = img.Rescale(12, 12, wxIMAGE_QUALITY_HIGH);
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