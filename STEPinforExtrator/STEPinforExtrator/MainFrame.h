#pragma once
#include<wx/wx.h>
#include <wx/dirctrl.h>
#include <wx/treectrl.h>

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    void OnOpenFolder(wxCommandEvent& event);
    wxGenericDirCtrl* m_dirCtrl;
};
