//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("Prichedit1.res");
USEFORM("Urichedit.cpp", Form1);
USE("Prichedit1.todo", ToDo);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();
    }
    catch (Exception &exception)
    {
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
