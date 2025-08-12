#ifndef _APP_HELPER_HPP_
#define _APP_HELPER_HPP_
#include "wxInclude.hpp"
#include "view/MainViewPort.hpp"
#include "ui/ElementTreeCtrl.hpp"
namespace threepp
{
	class Scene;
}
namespace dragon
{
	class WindowFrame;
	class BimRenderCanvas; 
	static class FixedSizeImpl : public wxBitmapBundleImpl
	{
	public:
		FixedSizeImpl(const wxSize& sizeDef, const wxIcon& icon)
			: m_sizeDef(sizeDef),
			m_icon(icon)
		{
		}

		wxSize GetDefaultSize() const override
		{
			return m_sizeDef;
		}

		wxSize GetPreferredBitmapSizeAtScale(double scale) const override
		{
			return m_sizeDef * scale;
		}

		wxBitmap GetBitmap(const wxSize& size) override
		{
			wxBitmap bmp(m_icon);
			if (size != bmp.GetSize())
				wxBitmap::Rescale(bmp, size);

			return bmp;
		}
	private:
		const wxSize m_sizeDef;
		const wxIcon m_icon;
	};

	class AppHelper
	{
	public:
		/*APP HELPER GET MAIN VIEW PORT SCENE*/
		static MainViewPort* getMainViewPortScene(WindowFrame* main_frame);
		static BimRenderCanvas* getRenderCanvas(WindowFrame* main_frame);
		static ElementTreeCtrl* getMainTreeCtrl(WindowFrame* main_frame);
		static wxBitmapBundle loadBitmapBundle(const std::string& path, wxBitmapType type);
		static void GetWindowSize(wxWindow* window, int& width, int& height); 
		static void GetFramebufferSize(wxWindow* window, int* width, int* height);
	private:
		AppHelper() = default;
	};
}
#endif // !_APP_HELPER_HPP_