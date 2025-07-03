#include "AppCommandsHandler.hpp"
#include "core/io/factory/FileHandlerFactory.hpp"
#include "core/io/IFileHandler.hpp"
#include "spdlog/spdlog.h"
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
			spdlog::info(std::format("Open file : {}", path.ToStdString()));
			file->setViewport(m_ParentWindow);
			file->open({ path.ToStdString() });
		}
	}
}