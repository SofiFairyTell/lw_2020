//---------------------------------------------------------------------------

#ifndef RecieveUnitH
#define RecieveUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ScktComp.hpp>
//---------------------------------------------------------------------------
class TRecieverForm : public TForm
{
__published:	// IDE-managed Components
        TClientSocket *Client;
        TSaveDialog *SaveDialog;
        TButton *ConnectButton;
        void __fastcall ClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ConnectButtonClick(TObject *Sender);
private:
 AnsiString name;
public:		// User declarations
        __fastcall TRecieverForm(TComponent* Owner);
};
void Write( AnsiString Text );
//---------------------------------------------------------------------------
extern PACKAGE TRecieverForm *RecieverForm;
//---------------------------------------------------------------------------
#endif
