//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "SplashScreen.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSplash *Splash;
//---------------------------------------------------------------------------
__fastcall TSplash::TSplash(TComponent* Owner)
        : TForm(Owner)
{
        Left = (Screen->Width - Width) /2;
        Top = (Screen->Height - Height) /2;
        Progress->Position=0;
        SetWindowPos(Handle, HWND_TOPMOST,Left,Top,Width,Height,NULL);
}
//---------------------------------------------------------------------------
void TSplash::SetProgress(int Max, ZXString text)
{
        Progress->Max=Max;
        Action->Caption="Initialising "+text;
}

void TSplash::IncProgress(ZXString text)
{
        Action->Caption="Initialising "+text+"...";
        Progress->Position ++;
        Application->ProcessMessages();
}






