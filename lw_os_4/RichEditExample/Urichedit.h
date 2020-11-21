//---------------------------------------------------------------------------
#ifndef UricheditH
#define UricheditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <AppEvnts.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TMainMenu *MainMenu1;
    TMenuItem *MFile;
    TMenuItem *MOpen;
    TMenuItem *MClose;
    TMenuItem *N5;
    TMenuItem *MSave;
    TMenuItem *MSaveAs;
    TMenuItem *N8;
    TMenuItem *MPrint;
    TMenuItem *MPrintSetup;
    TMenuItem *N12;
    TMenuItem *MExit;
    TMenuItem *N14;
	TMenuItem *MEdit;
	TMenuItem *MUndo;
    TMenuItem *N17;
	TMenuItem *MCut;
	TMenuItem *MCopy;
	TMenuItem *MPaste;
    TMenuItem *N21;
	TMenuItem *MFind;
	TMenuItem *MReplace;
    TMenuItem *N24;
    TMenuItem *MFont;
    TMenuItem *MAlign;
    TRichEdit *RichEdit1;
    TOpenDialog *OpenDialog1;
    TSaveDialog *SaveDialog1;
    TFontDialog *FontDialog1;
	TMenuItem *MCreate;
    TMenuItem *MLeft;
    TMenuItem *MRight;
    TMenuItem *MCenter;
    TMenuItem *MNumb;
	TStatusBar *StatusBar1;
	TFindDialog *FindDialog1;
	TReplaceDialog *ReplaceDialog1;
	TPrintDialog *PrintDialog1;
	TPrinterSetupDialog *PrinterSetupDialog1;
	TToolBar *ToolBar1;
	TToolButton *TBOpen;
	TToolButton *TBSave;
	TToolButton *ToolButton3;
	TToolButton *TBPrint;
	TToolButton *ToolButton5;
	TToolButton *TBExit;
	TImageList *ImageList1;
	TMenuItem *N11;
	TMenuItem *N13;
	TMenuItem *N15;
	TMenuItem *N16;
	TMenuItem *N18;
	TMenuItem *N19;
	TMenuItem *N20;
	TMenuItem *N22;
	TMenuItem *N23;
	TMenuItem *N25;
	TMenuItem *N26;
	TMenuItem *N27;
	TToolButton *TBCreate;
	TToolButton *TBFont;
	TToolButton *TBCenter;
	TToolButton *TBLeft;
	TToolButton *TBRight;
	TToolButton *TBFind;
	TToolButton *TBReplace;
	TToolButton *ToolButton1;
	TToolButton *TBCut;
	TToolButton *TBCopy;
	TToolButton *TBPaste;
	TToolButton *TBNumb;
        TApplicationEvents *ApplicationEvents1;
    void __fastcall MOpenClick(TObject *Sender);
    void __fastcall MSaveAsClick(TObject *Sender);
    void __fastcall MFontClick(TObject *Sender);
    
    void __fastcall MNumbClick(TObject *Sender);
    void __fastcall MLeftClick(TObject *Sender);
    
    void __fastcall MRightClick(TObject *Sender);
    void __fastcall MCenterClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);

	void __fastcall RichEdit1KeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall RichEdit1MouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	
	void __fastcall FormResize(TObject *Sender);
	
	
	void __fastcall MSaveClick(TObject *Sender);
	void __fastcall FontDialog1Apply(TObject *Sender, HWND Wnd);
	void __fastcall MFindClick(TObject *Sender);
	void __fastcall FindDialog1Find(TObject *Sender);
	void __fastcall MCopyClick(TObject *Sender);
	void __fastcall MCutClick(TObject *Sender);
	void __fastcall MPasteClick(TObject *Sender);
	void __fastcall MUndoClick(TObject *Sender);

	void __fastcall ReplaceDialog1Replace(TObject *Sender);
	void __fastcall MReplaceClick(TObject *Sender);
	void __fastcall MExitClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall MCreateClick(TObject *Sender);
	void __fastcall MPrintSetupClick(TObject *Sender);
	void __fastcall MPrintClick(TObject *Sender);
        void __fastcall ApplicationEvents1Hint(TObject *Sender);
	


	
private:
 // User declarations
 AnsiString FName;
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
    void __fastcall DisplayHint(TObject *Sender);
    void __fastcall StatusExecute(TObject *Sender);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
