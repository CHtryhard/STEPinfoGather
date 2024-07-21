#pragma once
#include<wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/treectrl.h>
#include <wx/textctrl.h>

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    void OnOpenFolder(wxCommandEvent& event);
    void OnCloseWindow(wxCommandEvent& event);
    //wxGenericDirCtrl* m_dirCtrl;
    wxTextCtrl* fileListTextCtrl;
};
