#include "notificationdialog.h"
#include "ui_notificationdialog.h"

NotificationDialog::NotificationDialog(QString tag_name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NotificationDialog)
{
    ui->setupUi(this);

    ui->message->setText("Installer update '" + tag_name + "' found.  Do you want to download the update?");
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->setWindowModality(Qt::WindowModality::WindowModal);

    connect(ui->Yes, &QPushButton::released, this, &NotificationDialog::on_yes);
    connect(this, &NotificationDialog::yes, this, &NotificationDialog::close_dialog);
    connect(ui->No, &QPushButton::released, this, &NotificationDialog::close_dialog);
}

NotificationDialog::~NotificationDialog()
{
    delete ui;
}

void NotificationDialog::on_yes()
{
    emit yes();
}

void NotificationDialog::close_dialog()
{
    NotificationDialog::close();
}