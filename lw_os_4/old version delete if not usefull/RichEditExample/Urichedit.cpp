//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Urichedit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
/* TODO 2 -oСидоров -cКод : Главное меню */
void __fastcall TForm1::MOpenClick(TObject *Sender)
{
if (OpenDialog1->Execute())
 {
  FName = OpenDialog1->FileName;
  RichEdit1->Lines->LoadFromFile(OpenDialog1->FileName);
  RichEdit1->Modified = false;
 }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MSaveAsClick(TObject *Sender)
{
SaveDialog1->FileName = FName;
if (SaveDialog1->Execute())
 {
  FName = OpenDialog1->FileName;
  RichEdit1->Lines->SaveToFile(SaveDialog1->FileName);
  RichEdit1->Modified = false;
 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MFontClick(TObject *Sender)
{
FontDialog1->Font->Assign(RichEdit1->SelAttributes);
if (FontDialog1->Execute())
   RichEdit1->SelAttributes->Assign(FontDialog1->Font);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MNumbClick(TObject *Sender)
{
 MNumb->Checked = ! MNumb->Checked;
 if (MNumb->Checked) RichEdit1->Paragraph->Numbering = nsBullet;
 else  RichEdit1->Paragraph->Numbering = nsNone;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MLeftClick(TObject *Sender)
{
 MLeft->Checked = true;
 RichEdit1->Paragraph->Alignment = taLeftJustify;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::MRightClick(TObject *Sender)
{
 MRight->Checked = true;
 RichEdit1->Paragraph->Alignment = taRightJustify;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MCenterClick(TObject *Sender)
{
 MCenter->Checked = true;
 RichEdit1->Paragraph->Alignment = taCenter;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
RichEdit1->Paragraph->FirstIndent = 0;
RichEdit1->Paragraph->LeftIndent = - RichEdit1->Paragraph->FirstIndent;
RichEdit1->Modified = false;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DisplayHint(TObject *Sender)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::StatusExecute(TObject *Sender)
{
 StatusBar1->Panels->Items[0]->Text =
                    IntToStr((int)RichEdit1->CaretPos.y+1) +
                    ": "+IntToStr((int)RichEdit1->CaretPos.x+1);
 if (RichEdit1->Modified)
  StatusBar1->Panels->Items[1]->Text = "модиф.";
  else StatusBar1->Panels->Items[1]->Text = "";
 switch (RichEdit1->Paragraph->Alignment)
 {
   case taLeftJustify: TBLeft->Down = true;
   break;
   case taCenter: TBCenter->Down = true;
   break;
   case taRightJustify: TBRight->Down = true;
   break;
 }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RichEdit1KeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 StatusExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RichEdit1MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 StatusExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormResize(TObject *Sender)
{
 StatusExecute(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MSaveClick(TObject *Sender)
{
if (FName != "")
 RichEdit1->Lines->SaveToFile(FName);
 else MSaveAsClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FontDialog1Apply(TObject *Sender, HWND Wnd)
{
   RichEdit1->SelAttributes->Assign(FontDialog1->Font);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MFindClick(TObject *Sender)
{
/* начальное значение текста поиска - текст, выделенный в
   RichEdit1 */
  FindDialog1->FindText = RichEdit1->SelText;
  FindDialog1->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FindDialog1Find(TObject *Sender)
{
  int FoundAt, StartPos, ToEnd;
  TSearchTypes Option;
  TReplaceDialog *dlg = (TReplaceDialog *)Sender;

/* если было выделение, то поиск идет начиная с его
   последнего символа, иначе - с позиции курсора */
  StartPos = RichEdit1->SelStart;
  if (RichEdit1->SelLength)
    StartPos += RichEdit1->SelLength;

/* ToEnd - длина текста, начиная с первой позиции поиска
   и до конца */
  ToEnd = RichEdit1->Text.Length() - StartPos;

/* поиск целого слова или нет в зависимости от установки
   пользователя */

  if (dlg->Options.Contains(frWholeWord))
   Option << stWholeWord;
  else Option >> stWholeWord;

/* поиск с учетом или без учета регистра в зависимости от
   установки пользователя */
  if (dlg->Options.Contains(frMatchCase))
   Option << stMatchCase;
  else Option >> stMatchCase;

  FoundAt = RichEdit1->FindText(dlg->FindText,
                                StartPos, ToEnd, Option);

  if (FoundAt != -1) // если найдено
  {
    RichEdit1->SelStart = FoundAt;
    RichEdit1->SelLength = dlg->FindText.Length();
    if ((dlg->Name == "ReplaceDialog1")&&(dlg->Options.Contains(frReplaceAll)))
      ReplaceDialog1Replace(Sender);
}
  else ShowMessage("Текст '" + dlg->FindText +
                   "' не найден");
    RichEdit1->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MCopyClick(TObject *Sender)
{
 RichEdit1->CopyToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MCutClick(TObject *Sender)
{
 RichEdit1->CutToClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MPasteClick(TObject *Sender)
{
 RichEdit1->PasteFromClipboard();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MUndoClick(TObject *Sender)
{
 RichEdit1->Undo();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::ReplaceDialog1Replace(TObject *Sender)
{
 RichEdit1->SetFocus();
 if (RichEdit1->SelText != "")
  {
   RichEdit1->SelText = ReplaceDialog1->ReplaceText;
   if (ReplaceDialog1->Options.Contains(frReplaceAll))
     RichEdit1->SelStart += ReplaceDialog1->ReplaceText.Length();
  }
 else if (ReplaceDialog1->Options.Contains(frReplace))
  {
   ShowMessage("Текст '" + ReplaceDialog1->FindText +
                   "' не найден");
   return;
  }
 if (ReplaceDialog1->Options.Contains(frReplaceAll))
   FindDialog1Find(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TForm1::MReplaceClick(TObject *Sender)
{
/* начальное значение текста поиска - текст, выделенный в
   RichEdit1 */
  ReplaceDialog1->FindText = RichEdit1->SelText;
  ReplaceDialog1->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MExitClick(TObject *Sender)
{
 Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{
 if (RichEdit1->Modified)
 {
  int res = MessageDlg("Текст в окне был изменен. Сохранить его?",
                 mtConfirmation,
                 TMsgDlgButtons() << mbYes<< mbNo << mbCancel,0);
  switch (res)
      {
        case mrYes:    MSaveClick(Sender);
        			   break;
        case mrCancel: CanClose = false;
      }
  }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MCreateClick(TObject *Sender)
{
 if (RichEdit1->Modified)
 {
  int res = MessageDlg("Текст в окне был изменен. Сохранить его?",
                 mtConfirmation,
                 TMsgDlgButtons() << mbYes<< mbNo << mbCancel,0);
  switch (res)
      {
        case mrYes:    MSaveClick(Sender);
        			   break;
        case mrCancel: return;
      }
  }
 RichEdit1->Clear();
 FName = "";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MPrintSetupClick(TObject *Sender)
{
 PrinterSetupDialog1->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::MPrintClick(TObject *Sender)
{
 if (PrintDialog1->Execute())
  for (int i = 1; i <= PrintDialog1->Copies; i++)
   RichEdit1->Print("Печать PRichEdit1");
}
//---------------------------------------------------------------------------







void __fastcall TForm1::ApplicationEvents1Hint(TObject *Sender)
{
  StatusBar1->Panels->Items[2]->Text = Application->Hint;
       
}
//---------------------------------------------------------------------------

