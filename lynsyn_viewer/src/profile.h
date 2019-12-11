#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include <QFile>
#include <QtSql>

#include "profiledialog.h"

class Profile : public QObject {
  Q_OBJECT

private:
  QString dbFilename;
  ProfileDialog *profDialog;

public:
  QString dbConnection;

  Profile(QString dbFilename);
  ~Profile();
  void connect();
  void disconnect();

  bool importCsv(QString csvFilename, QStringList elfFilenames, QString kallsyms);
  bool exportCsv(QString csvFilename);
  void clean();

  bool initProfiler(bool *useJtag);
  bool endProfiler();
  void setParameters(ProfileDialog *profDialog);
  unsigned cores();
  unsigned sensors();

public slots:
  bool runProfiler();
    
signals:
  void advance(int step, QString msg);
  void finished(int ret, QString msg);
};

#endif
