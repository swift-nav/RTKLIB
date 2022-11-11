//---------------------------------------------------------------------------
#ifndef getoptdlgH
#define getoptdlgH
//---------------------------------------------------------------------------
#include "ui_getoptdlg.h"
#include <QDialog>

//---------------------------------------------------------------------------
class DownOptDialog : public QDialog, private Ui::DownOptDialog {
  Q_OBJECT

protected:
  void showEvent(QShowEvent *);

public slots:
  void BtnOkClick();
  void BtnUrlFileClick();
  void BtnLogFileClick();

public:
  explicit DownOptDialog(QWidget *parent);
};
//---------------------------------------------------------------------------
#endif
