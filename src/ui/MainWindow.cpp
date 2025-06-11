#include "MainWindow.hpp"
#include "WindowFrame.hpp"

namespace dragon
{
	bool App::OnInit()
	{
		if (!m_Frame)
		{
			m_Frame = new WindowFrame();
		}
		m_Frame->Show();
		return true;
	}
}