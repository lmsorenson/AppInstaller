#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QDebug>

progressdialog::progressdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progressdialog)
{
    ui->setupUi(this);
}

progressdialog::progressdialog(const progressdialog &progressdialog)
{
    ui->setupUi(this);
}

void progressdialog::add_progress(int progress)
{
    qDebug() << "Received progress: " << progress;
    ui->progressBar->setValue(progress);
}

progressdialog::~progressdialog()
{
    delete ui;
}
