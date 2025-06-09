#include "MainWindow.hpp"
#include "WindowFrame.hpp"

namespace viewer
{
	bool MainWindow::OnInit()
	{
		if (!m_Frame)
		{
			m_Frame = new WindowFrame();
		}
		m_Frame->Show();
		return true;
	}
}