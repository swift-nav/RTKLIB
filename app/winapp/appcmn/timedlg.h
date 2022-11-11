//---------------------------------------------------------------------------
#ifndef timedlgH
#define timedlgH
//---------------------------------------------------------------------------
#include "rtklib.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
class TTimeDialog : public TForm {
  __published : TButton *BtnOk;
  TLabel *Message;
  void __fastcall FormShow(TObject *Sender);

private:
public:
  gtime_t Time;
  __fastcall TTimeDialog(TComponent *Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TTimeDialog *TimeDialog;
//---------------------------------------------------------------------------
#endif
