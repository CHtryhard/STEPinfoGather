#include "MainFrame.h"
#include <wx/dirctrl.h>
#include <wx/treectrl.h>
#include <wx/dir.h>
#include <fstream>
#include "STEP__Converter.h"

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 400))
{   
    wxPanel* panel = new wxPanel(this);
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_OPEN, "&Open Folder...\tCtrl-O", "Open a folder");
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");


    wxButton* confirmButton = new wxButton(panel, wxID_ANY, "Confrim", wxPoint(500, 20), wxSize(80, 80));
    wxButton* cancelButton = new wxButton(panel, wxID_ANY, "Cancel", wxPoint(500, 120), wxSize(80, 80));

    fileListTextCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxPoint(20, 20), wxSize(460, 340), wxTE_MULTILINE | wxTE_READONLY);

    SetMenuBar(menuBar);
    CreateStatusBar();
    
    //Bind event
    Bind(wxEVT_MENU, &MyFrame::OnOpenFolder, this, wxID_OPEN);
    cancelButton->Bind(wxEVT_BUTTON, &MyFrame::OnCloseWindow, this);
    confirmButton->Bind(wxEVT_BUTTON, &MyFrame::OnConfirmButton, this);
}

void MyFrame::OnOpenFolder(wxCommandEvent& event)
{
    wxDirDialog openDirDialog(this, "Choose a directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (openDirDialog.ShowModal() == wxID_CANCEL)
        return;
    selectedPath = openDirDialog.GetPath();
    wxLogStatus("Folder selected: %s", selectedPath);

    wxDir dir(selectedPath);

    if (!dir.IsOpened())
    {
        wxLogError("Could not open directory: %s", selectedPath);
        return;
    }

    wxString filename;

    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
    while (cont)
    {
        fileListTextCtrl->AppendText(filename + "\n");
        cont = dir.GetNext(&filename);
    }

}

void MyFrame::OnCloseWindow(wxCommandEvent& event)
{
    wxLogMessage("Exit now");
    Close(true);
}

void MyFrame::OnConfirmButton(wxCommandEvent& event)
{
    std::string path;
    path = selectedPath.ToStdString();
    
    if (processStepFile(path) == -1) {
        wxLogMessage("Error reading STEP file");
    }
    else {
        wxLogMessage("Success");
    }
}