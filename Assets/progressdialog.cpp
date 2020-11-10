#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>

progressdialog::progressdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progressdialog)
{
    ui->setupUi(this);

    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowModality(Qt::WindowModality::WindowModal);
}

progressdialog::progressdialog(const progressdialog &progressdialog)
{
    ui->setupUi(this);
}

void progressdialog::add_progress(int progress)
{
    ui->label->setText(this->windowTitle());
    qDebug() << "Received progress: " << progress;
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


