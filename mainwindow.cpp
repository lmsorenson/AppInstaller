#include <QStringListModel>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

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

    this->setWindowTitle(title_);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->InstallDirectoryDisplay->setText(install_directory);
    ui->AssetNameDisplay->setText(asset_name);

    ui->install->setDefault(true);
    ui->install->setDisabled(true);
    ui->use->setDisabled(true);
    ui->remove->setDisabled(true);

    connect(ui->install, &QPushButton::released, this, &MainWindow::on_install);
    connect(ui->use, &QPushButton::released, this, &MainWindow::on_use);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::provide_assets(QStringList asset_ids)
{
    qDebug() << "MAIN WINDOW: Assets received.";

    QStringListModel * model = new QStringListModel(this);
    QStringList list;

    for (auto item : asset_ids)
    {
        qDebug() << "MAIN WINDOW: adding asset " << item;
        list << item;
    }

    list.removeDuplicates();
    list.sort();

    model->setStringList( list );
    ui->listView->setModel( model );

    if (ui->listView->selectionModel() != nullptr)
        connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_selection_changed(QItemSelection)));

    qDebug() << "Finished processing found assets."
}

void MainWindow::on_selection_changed(const QItemSelection& selection)
{
    if (selection.indexes().size() == 1)
    {
        auto model_index = selection.indexes().takeFirst();
        if (model_index.isValid())
        {
            auto data = model_index.data().value<QString>();
            qDebug() << "Selection changed to: " << data;

            emit validate_actions(data);
        }
    }
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

void MainWindow::on_install()
{
    auto selectedIndex = ui->listView->currentIndex();

    if (selectedIndex.isValid())
    {
        auto data = selectedIndex.data().value<QString>();

        qDebug() << "Attempting to install: " << data;

        emit install(data);
    }
}

void MainWindow::on_use()
{
    auto selectedIndex = ui->listView->currentIndex();

    if (selectedIndex.isValid())
    {
        auto data = selectedIndex.data().value<QString>();

        qDebug() << "Attempting to setup: " << data;

        emit use(data);
    }
}
