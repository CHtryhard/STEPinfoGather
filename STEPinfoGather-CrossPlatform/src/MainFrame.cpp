#include "MainFrame.h"
#include <wx/dirctrl.h>
#include <wx/treectrl.h>
#include <wx/dir.h>
#include "STEP__Converter.h"
#include <string>
#include <fstream>
#include <ctime>
#include <cctype>

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
    fileListTextCtrl->Clear();
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
    std::string c_path;
    c_path = selectedPath.ToStdString();

    wxDir dir(selectedPath);

    wxString filename;
    std::string c_filename;
    std::string c_filepath;
    wxArrayString patterns;
    patterns.Add("*.step");
    patterns.Add("*.stp");
    patterns.Add("*.STEP");
    patterns.Add("*.STP");
    patterns.Add("*.Step");
    patterns.Add("*.Stp");

    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);

    //initialize file to write
    std::ofstream outputfile;
    std::time_t currentTime = std::time(nullptr);
    // Initialize a tm structure
    std::tm* localTime = std::localtime(&currentTime);
    // Extract the year, month, day, hour, minute
    std::ostringstream oss;
    oss << std::put_time(localTime,"%Y-%m-%d__%H%M");
    std::string timestamp = oss.str();
    std::string outputpath = c_path + "/" + timestamp + ".csv";
    outputfile.open(outputpath, std::ios::app);
    //write header
    outputfile << "File name,X,Y,Z,BV/5000,SV,Density,Weight,QTY" << std::endl;

    while (cont)
    {
        if (MatchesPattern(filename, patterns)) {
        wxLogMessage(filename);
        c_filename = filename.ToStdString();
        c_filepath = c_path +"/"+ c_filename;
        processStepFile(c_filepath, c_filename, outputfile);
        }
        cont = dir.GetNext(&filename);
    }

    outputfile.close();
}