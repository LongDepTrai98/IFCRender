#include "MainWindow.hpp"
#include "WindowFrame.hpp"

namespace dragon
{
	bool App::OnInit()
	{
		this->SetAppearance(Appearance::System);
		wxInitAllImageHandlers();
		if (!m_Frame)
		{
			m_Frame = new WindowFrame();
		}
		m_Frame->Show();
		return true;
	}
}