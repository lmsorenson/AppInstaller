#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QJsonDocument>

progressdialog::progressdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progressdialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);

    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowModality(Qt::WindowModality::WindowModal);
}

progressdialog::progressdialog(const progressdialog &progressdialog)
{
    ui->setupUi(this);
}

void progressdialog::set_title(QString title)
{
    ui->label->setText("Downloading " + title + ":");
}

void progressdialog::add_progress(int progress)
{
    ui->progressBar->setValue(progress);
}

progressdialog::~progressdialog()
{
    this->setWindowModality(Qt::WindowModality::NonModal);
    delete ui;
}

void progressdialog::close_dialog()
{
    progressdialog::close();
}


