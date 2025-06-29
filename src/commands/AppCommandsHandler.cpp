#include "AppCommandsHandler.hpp"
#include "core/io/FileHandlerFactory.hpp"
#include "core/io/IFileHandler.hpp"
namespace dragon
{
	AppCommandHandler::AppCommandHandler(wxWindow* parent) : m_ParentWindow(parent)
	{
	}
	void AppCommandHandler::OnOpenFile(wxCommandEvent& event)
	{
		wxFileDialog dlg(m_ParentWindow, "Open file", "", "", "*.*", wxFD_OPEN);
		if (dlg.ShowModal() == wxID_OK) {
			wxString path = dlg.GetPath();
			std::unique_ptr<FileHandler> file = FileHandlerFactory::create({ path.ToStdString() });
			if (!file)
			{
				wxMessageBox("Read file error");
				return;
			}
			file->setViewport(m_ParentWindow);
			file->open({ path.ToStdString() });
		}
	}
}