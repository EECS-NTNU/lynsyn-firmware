#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include <QDialog>

#include "ui_profiledialog.h"

namespace Ui {
  class ProfileDialog;
}

class ProfileDialog : public QDialog {
  Q_OBJECT

public:
  Ui::ProfileDialog *ui;

  unsigned numCores;
  unsigned numSensors;
  bool hasVoltageSensors;
  bool useJtag; 

  QVector<QCheckBox*> coreCheckboxes;
  QVector<QDoubleSpinBox*> sensorSpinboxes;

  explicit ProfileDialog(unsigned numCores, bool hasVoltageSensors, unsigned numSensors, bool useJtag, QWidget *parent = 0);
  ~ProfileDialog();
};

#endif // PROFILEDIALOG_H
