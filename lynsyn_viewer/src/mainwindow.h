#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "lynsyn_viewer.h"
#include "graphscene.h"
#include "graphview.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

  Profile *profile;
  GraphScene *graphScene;
  GraphView *graphView;
  QComboBox *measurementBox;
  QComboBox *coreBox;
  QComboBox *sensorBox;
  ProfileDialog *profDialog;

  QThread thread;
  QProgressDialog *progDialog;

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void about();
  void importCsv();
  void exportCsv();
  void upgrade();
  void profileEvent();
  void finishProfile(int error, QString msg);
  void advance(int step, QString msg);
  void changeCore(int core);
  void changeSensor(int sensor);
  void changeMeasurement(int measurement);

protected:
  void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H