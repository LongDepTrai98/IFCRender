#include "MainWindow.hpp"
#include "WindowFrame.hpp"

namespace dragon
{
	bool App::OnInit()
	{
		SetExitOnFrameDelete(true);
		wxHandleFatalExceptions(false);
		this->SetAppearance(Appearance::System);
		wxInitAllImageHandlers();
		if (!m_Frame)
		{
			m_Frame = new WindowFrame();
		}
		m_Frame->Show();
		return true;
	}
	int App::OnRun()
	{
		try {
			return wxApp::OnRun();
		}
		catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
			throw; 
		}
	}
}