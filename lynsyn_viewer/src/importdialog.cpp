#include <QFileDialog>
#include <QSettings>

#include "importdialog.h"
#include "ui_importdialog.h"

ImportDialog::ImportDialog(QWidget *parent) : QDialog(parent), ui(new Ui::ImportDialog) {
  QSettings settings;

  ui->setupUi(this);

  ui->csvEdit->setText(settings.value("csv", "output.csv").toString());
  ui->elfEdit->setText(settings.value("elf", "").toString());
  ui->kallsymsEdit->setText(settings.value("kallsyms", "").toString());
}

ImportDialog::~ImportDialog() {
  QSettings settings;

  settings.setValue("csv", ui->csvEdit->text());
  settings.setValue("elf", ui->elfEdit->text());
  settings.setValue("kallsyms", ui->kallsymsEdit->text());

  delete ui;
}

void ImportDialog::updateCsv() {
  QFileDialog dialog(this, "Select CSV file to import");
  dialog.setNameFilter(tr("CSV (*.csv)"));
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  if(dialog.exec()) {
    ui->csvEdit->setText(dialog.selectedFiles()[0]);
  }
}
