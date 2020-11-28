#include <QStringListModel>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>


MainWindow::MainWindow(QString project_name, QString install_directory, QString asset_name, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , title_(project_name + " Installer")
{
    ui->setupUi(this);

    interface_ = new PackageInterface(this);
    interface_->set_view(ui->listView);

    self_ = new PackageInterface(this);

    this->setWindowTitle(title_);
    ui->InstallDirectoryDisplay->setText(install_directory);
    ui->AssetNameDisplay->setText(asset_name);

    ui->install->setDefault(true);
    ui->install->setDisabled(true);
    ui->use->setDisabled(true);
    ui->remove->setDisabled(true);

    if (interface_)
    {
        connect(ui->install, &QPushButton::released, interface_, &PackageInterface::on_install);
        connect(ui->use, &QPushButton::released, interface_, &PackageInterface::on_use);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

const PackageInterface * const MainWindow::get_interface()
{
    return interface_;
}

const PackageInterface * const MainWindow::get_self_interface()
{
    return self_;
}

void MainWindow::on_selected_install_exists(bool install_exists)
{
    if (install_exists)
    {
        ui->install->setDisabled(true);
        ui->use->setDisabled(false);
    }
    else
    {
        ui->install->setDisabled(false);
        ui->use->setDisabled(true);
    }
}
