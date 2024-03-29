//---------------------------------------------------------------------------

#ifndef SplashScreenH
#define SplashScreenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include "zx81config.h"
//---------------------------------------------------------------------------
class TSplash : public TForm
{
__published:	// IDE-managed Components
        TImage *Image1;
        TProgressBar *Progress;
        TLabel *Action;
private:	// User declarations
public:		// User declarations
        __fastcall TSplash(TComponent* Owner);
        void SetProgress(int Max, ZXString text);
        void IncProgress(ZXString text);
};
//---------------------------------------------------------------------------
extern PACKAGE TSplash *Splash;
//---------------------------------------------------------------------------
#endif
