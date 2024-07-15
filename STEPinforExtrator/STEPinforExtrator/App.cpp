#include "App.h"
#include "MainFrame.h"
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame("STEP files - Analyzer");
    frame->Show(true);
    return true;
}