#include "AppCommandsHandler.hpp"
#include "core/io/factory/FileHandlerFactory.hpp"
#include "core/io/IFileHandler.hpp"
#include "spdlog/spdlog.h"
#include "ui/WindowFrame.hpp"
#include "ui/RenderCanvas.hpp"
#include "input/input.hpp"
namespace dragon
{
	AppCommandHandler::AppCommandHandler(wxWindow* parent) : m_ParentWindow(parent)
	{
	}
	void AppCommandHandler::OnIconize(wxIconizeEvent& event)
	{
		WindowFrame* main_frame = static_cast<WindowFrame*>(m_ParentWindow);
		main_frame->getRenderCanvas()->Invalidate();
		if (event.IsIconized())
		{
			/*MINIMIZE*/
		}
		else
		{
			/*RESTORE*/
		}
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
	void AppCommandHandler::OnToolbarClick(wxCommandEvent& event)
	{
		int toolId = event.GetId();
		WindowFrame* main_frame = static_cast<WindowFrame*>(m_ParentWindow);
		wxToolBar* toolbar = main_frame->GetToolBar();
		bool toolbarState{ false };
		if (toolbar)
		{
			toolbarState = toolbar->GetToolState(toolId);
		}
		ToolBarData data(event, toolbarState);
		main_frame->OnCallbackToolbarCommand(data);
		event.Skip();
	}
}