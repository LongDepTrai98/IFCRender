#ifndef _IFC_dragon_HPP_
#define _IFC_dragon_HPP_
#include "wxInclude.hpp"
namespace dragon
{
	class WindowFrame;
	class App : public wxApp
	{
	public:
		bool OnInit() override;
	private:
		WindowFrame* m_Frame{ nullptr };
	};
}
#endif // !_IFC_dragon_HPP_