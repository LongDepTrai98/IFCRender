#include "MainWindow.hpp"
#include "WindowFrame.hpp"

namespace viewer
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