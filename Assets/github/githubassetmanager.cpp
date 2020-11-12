#include "githubassetmanager.h"
#include <QNetworkAccessManager>
#include <QtConcurrent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>

#include <Assets/download.h>
#include <Archives/zippackage.h>

GitHubAssetManager::GitHubAssetManager(QWidget *parent)
: AssetManagerBase(parent)
, github_username_("lmsorenson")
, github_project_("AgCabLab")
, github_asset_name_("AgCab")
, github_token_("e1d59c7b6764186b9a4adca957a7dadead2e4ccf")
, install_directory_("/Users/Shared/AgCab")
{
}

QFuture<QStringList> GitHubAssetManager::request_asset_ids()
{

    QNetworkRequest request(QUrl("https://api.github.com/repos/" + github_username_ + "/" + github_project_ + "/releases"));
    request.setRawHeader("Authorization", QString("token " + github_token_).toStdString().c_str());
    network_.get(request);

    QEventLoop loop;
    QObject::connect(&network_, &QNetworkAccessManager::finished, this, &GitHubAssetManager::on_assets_received);


    return QFuture<QStringList>();
}

void GitHubAssetManager::on_assets_received(QNetworkReply *reply)
{
    QStringList list;
    request_map_.clear();

    if (reply->error() == QNetworkReply::NoError) {
        QString reply_string = (QString) reply->readAll();

        //parse json
        QJsonDocument jsonResponse = QJsonDocument::fromJson(reply_string.toUtf8());

        if (jsonResponse.isArray()) {
            auto array = jsonResponse.array();

            for (auto itr = array.begin(); itr != array.end(); itr++) {
                auto item = itr->toObject();

                auto assets = item["assets"].toArray();
                for (auto it = assets.begin(); it != assets.end(); it++) {
                    auto asset = it->toObject();
                    if (asset["name"] == (github_asset_name_ + ".zip")) {
                        request_map_.insert(item["tag_name"].toString(), asset["url"].toString());
                        list << item["tag_name"].toString();
                    }
                }
            }
        }

        emit provide_asset_ids(list);

        delete reply;
    } else {
        qDebug() << "Failure: " << reply->errorString();
        delete reply;
    }
}



QFuture<QString> GitHubAssetManager::download_asset(QString asset_id, QString url)
{
    qDebug() << "download initiated for Tag: " << asset_id << "at REQUEST URL: " << url;

    active_download_ = new Download(install_directory_, github_asset_name_, asset_id, url, github_token_, dynamic_cast<QWidget*>(this->parent()));

    return active_download_->run();
}

QFuture<void> GitHubAssetManager::unzip_asset(QString file_name)
{
    active_archive_ = new ZipPackage(install_directory_, file_name, ".zip");

    return active_archive_->extract();
}