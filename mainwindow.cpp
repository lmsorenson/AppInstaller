#include <QStringListModel>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , title_("AgCabLab Installer")
{
    ui->setupUi(this);

    this->setWindowTitle(title_);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->install, &QPushButton::released, this, &MainWindow::on_install);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_selection_changed(const QItemSelection& selection)
{
    qDebug() << "Selection changed: " << selection.indexes();
}

void MainWindow::provide_assets(QStringList asset_ids)
{
    QStringListModel * model = new QStringListModel(this);
    QStringList list;

    for (auto item : asset_ids)
    {
        list << item;
    }

    list.removeDuplicates();
    list.sort();

    model->setStringList( list );
    ui->listView->setModel( model );
    connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_selection_changed(QItemSelection)));
}

void MainWindow::on_install()
{
    auto selectedIndex = ui->listView->currentIndex();

    auto model = dynamic_cast<QStringListModel*>(ui->listView->model());
    auto data = model->itemData(selectedIndex);
    auto tag = data.first().value<QString>();

    emit install(tag);
}


