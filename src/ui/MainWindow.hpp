#ifndef _IFC_VIEWER_HPP_
#define _IFC_VIEWER_HPP_
#include <wx/wx.h>
namespace viewer
{
	class WindowFrame; 
	class MainWindow : public wxApp
	{
	public: 
		bool OnInit() override;
	private: 
		WindowFrame* m_Frame{ nullptr }; 
	};
}
#endif // !_IFC_VIEWER_HPP_
