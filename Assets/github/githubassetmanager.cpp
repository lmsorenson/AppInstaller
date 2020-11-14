#include "githubassetmanager.h"
#include <QNetworkAccessManager>
#include <QtConcurrent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>
#include <QFile>

#include <unistd.h>

#include <Assets/download.h>
#include <Archives/zippackage.h>

GitHubAssetManager::GitHubAssetManager(QString install_directory, GitHubProject project, QWidget *parent)
: AssetManagerBase(install_directory + ((install_directory.endsWith("/")) ? "" : "/"), parent)
, github_username_(project.user_name)
, github_project_(project.project_name)
, github_asset_name_(project.asset_name)
, github_token_(project.access_token)
{
    QObject::connect(&network_, &QNetworkAccessManager::finished, this, &GitHubAssetManager::on_assets_received);
}

QString GitHubAssetManager::generate_installation_name(QString tag)
{
    return (github_asset_name_ + tag);
}

void GitHubAssetManager::request_asset_ids()
{
    QNetworkRequest request(QUrl("https://api.github.com/repos/" + github_username_ + "/" + github_project_ + "/releases"));
    request.setRawHeader("Authorization", QString("token " + github_token_).toStdString().c_str());
    network_.get(request);
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

    QObject::connect(active_download_, &Download::make_progress, progress_dialog_, &progressdialog::add_progress);

    return active_download_->run();
}

void GitHubAssetManager::download_cleanup()
{
    qDebug() << "The GitHub asset manager completed a download.";

    QObject::disconnect(active_download_, &Download::make_progress, nullptr, nullptr);
    delete active_download_;
    active_download_ = nullptr;
}

QFuture<QString> GitHubAssetManager::unzip_asset(QString file_name)
{
    active_archive_ = new ZipPackage(install_directory_, file_name, ".zip");

    return active_archive_->extract();
}

void GitHubAssetManager::unzip_cleanup(int result_index)
{
    qDebug() << "The GitHub asset manager finished an unzip operation. ";

    delete active_archive_;
    active_archive_ = nullptr;

    AssetManagerBase::unzip_cleanup(result_index);
}

void GitHubAssetManager::use_asset(QString directory_name)
{

    auto command = QString(directory_name + "/AgCab.app");
    qDebug() << "Use: " << command;

    auto link_name = QString(install_directory_ + "AgCabLab");

    if (!link_name.isEmpty())
    {
        QFile file(link_name);
        file.remove();
    }

    symlink(command.toStdString().c_str(), link_name.toStdString().c_str());
}