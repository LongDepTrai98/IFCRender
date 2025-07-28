#ifndef _IFC_dragon_HPP_
#define _IFC_dragon_HPP_
#include "wxInclude.hpp"
#include <wx/sysopt.h>
namespace dragon
{
	class WindowFrame;
	class App : public wxApp
	{
	public:
		App()
		{
			wxSystemOptions::SetOption("catch-unhandled-exceptions", 0);
		};
	public:
		bool OnInit() override;
		int OnRun() override;
	private:
		WindowFrame* m_Frame{ nullptr };
	};
}
#endif // !_IFC_dragon_HPP_