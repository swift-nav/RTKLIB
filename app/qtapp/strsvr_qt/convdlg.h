//---------------------------------------------------------------------------
#ifndef convdlgH
#define convdlgH
//---------------------------------------------------------------------------
#include "ui_convdlg.h"
#include <QDialog>

class QShowEvent;
//---------------------------------------------------------------------------
class ConvDialog : public QDialog, private Ui::ConvDialog {
  Q_OBJECT
public slots:

  void BtnOkClick();
  void ConversionClick();

protected:
  void showEvent(QShowEvent *);

private:
  void UpdateEnable(void);

public:
  QString ConvMsg, ConvOpt, AntType, RcvType;
  int ConvEna, ConvInp, ConvOut, StaId;
  double AntPos[3], AntOff[3];

  explicit ConvDialog(QWidget *parent);
};
//---------------------------------------------------------------------------
#endif
