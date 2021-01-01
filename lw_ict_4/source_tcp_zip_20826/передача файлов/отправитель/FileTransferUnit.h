//---------------------------------------------------------------------------

#ifndef FileTransferUnitH
#define FileTransferUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ScktComp.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TTransferForm : public TForm
{
__published:	// IDE-managed Components
        TOpenDialog *OpenDialog;
        TButton *StartButton;
        TButton *SendButton;
        TServerSocket *Server;
        void __fastcall StartButtonClick(TObject *Sender);
        void __fastcall SendButtonClick(TObject *Sender);
        void __fastcall ServerClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
private:	// User declarations
public:		// User declarations
        __fastcall TTransferForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTransferForm *TransferForm;
//---------------------------------------------------------------------------
#endif
