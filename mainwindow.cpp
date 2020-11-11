#include <QStringListModel>
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QNetworkReply>
#include <QDebug>
#include <QtConcurrent>

#include <Assets/download.h>
#include <Zip/zippackage.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("AgCabLab Installer");
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(&watcher_, &QFutureWatcher<void>::finished, this, &MainWindow::on_new_zip);
    connect(ui->install, &QPushButton::released, this, &MainWindow::on_install);
    connect(&network_, &QNetworkAccessManager::finished, this, &MainWindow::on_network_connection_made);

    QNetworkRequest request(QUrl("https://api.github.com/repos/lmsorenson/AgCabLab/releases"));
    request.setRawHeader("Authorization", "token e1d59c7b6764186b9a4adca957a7dadead2e4ccf");
    network_.get(request);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_network_connection_made(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QString reply_string = (QString)reply->readAll();

        //parse json
        QJsonDocument jsonResponse = QJsonDocument::fromJson(reply_string.toUtf8());

        if (jsonResponse.isArray())
        {
            auto array = jsonResponse.array();

            for( auto itr = array.begin(); itr != array.end(); itr++)
            {
                auto item = itr->toObject();

                auto assets = item["assets"].toArray();
                for( auto it = assets.begin(); it != assets.end(); it++)
                {
                    auto asset = it->toObject();
                    if (asset["name"] == "AgCab.zip")
                        map_.insert(item["tag_name"].toString(), asset["url"].toString());
                }
            }
        }

        delete reply;
    }
    else
    {
        qDebug() << "Failure: " << reply->errorString();
        delete reply;
    }

    QStringListModel * model = new QStringListModel(this);
    QStringList list;
    for (auto itr = map_.begin(); itr != map_.end(); itr++)
    {
        list << itr.key();
    }

    model->setStringList( list );
    ui->listView->setModel( model );
    connect(ui->listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_selection_changed(QItemSelection)));
}


void MainWindow::on_selection_changed(const QItemSelection& selection)
{
    qDebug() << "Selection changed: " << selection.indexes();
}

void MainWindow::on_install()
{
    if (!future_.isRunning())
    {
        auto selectedIndex = ui->listView->currentIndex();

        auto model = dynamic_cast<QStringListModel*>(ui->listView->model());
        auto data = model->itemData(selectedIndex);
        auto tag = data.first().value<QString>();
        auto url = map_[tag];

        active_download_ = new Download(tag, "AgCab", url, "e1d59c7b6764186b9a4adca957a7dadead2e4ccf", this);

        future_ = active_download_->run();

        if (!future_.isFinished())
            watcher_.setFuture(future_);

    }
}

void MainWindow::on_new_zip()
{
    auto selectedIndex = ui->listView->currentIndex();

    auto model = dynamic_cast<QStringListModel*>(ui->listView->model());
    auto data = model->itemData(selectedIndex);
    auto tag = data.first().value<QString>();

    QString filename = "AgCab" + tag;

    qDebug() << "new zip available: " << filename;

    auto package = new ZipPackage("/Users/Shared/AgCab", filename, ".zip");
    package->extract();
}


