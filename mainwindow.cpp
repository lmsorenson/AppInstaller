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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
        QString strReply = (QString)reply->readAll();

        //parse json
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());

        auto response = jsonResponse.isArray();

        auto array = jsonResponse.array();

        for( auto itr = array.begin(); itr != array.end(); itr++)
        {
            auto item = itr->toObject();

            auto assets = item["assets"].toArray();
            for( auto it = assets.begin(); it != assets.end(); it++)
            {
                auto asset = it->toObject();
                if (asset["name"] == "AgCab.zip")
                    map_.insert(item["tag_name"].toString(), asset["browser_download_url"].toString());
            }
        }
        delete reply;
    }
    else {
        //failure
        qDebug() << "Failure" <<reply->errorString();
        delete reply;
    }


    // Create model
    QStringListModel * model = new QStringListModel(this);

    // Make data
    QStringList list;

    for (auto itr = map_.begin(); itr != map_.end(); itr++)
    {
        list << itr.key();
    }

    // Populate our model
    model->setStringList(list);

    // Glue model and view together
    ui->listView->setModel(model);

    // Add additional feature so that
    // we can manually modify the data in ListView
    // It may be triggered by hitting any key or double-click etc.
    ui->listView->
            setEditTriggers(QAbstractItemView::AnyKeyPressed |
                            QAbstractItemView::DoubleClicked);
}

