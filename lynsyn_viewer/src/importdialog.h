#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>

namespace Ui {
  class ImportDialog;
}

class ImportDialog : public QDialog {
  Q_OBJECT

public:
  Ui::ImportDialog *ui;

  explicit ImportDialog(QWidget *parent = 0);
  ~ImportDialog();

public slots:
  void updateCsv();
};

#endif // IMPORTDIALOG_H
