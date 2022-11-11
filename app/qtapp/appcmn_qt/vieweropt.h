//---------------------------------------------------------------------------

#ifndef vieweroptH
#define vieweroptH
//---------------------------------------------------------------------------
#include "ui_vieweropt.h"
#include <QDialog>

//---------------------------------------------------------------------------
class ViewerOptDialog : public QDialog, private Ui::ViewerOptDialog {
  Q_OBJECT

public slots:
  void BtnColor1Click();
  void BtnColor2Click();
  void BtnFontClick();
  void BtnOkClick();

protected:
  void showEvent(QShowEvent *);
  QString color2String(const QColor &c);

public:
  explicit ViewerOptDialog(QWidget *parent);

  QFont Font;
  QColor Color1, Color2;
};
//---------------------------------------------------------------------------
#endif
