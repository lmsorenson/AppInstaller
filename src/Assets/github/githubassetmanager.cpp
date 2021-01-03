#include "githubassetmanager.h"
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QTimer>
#include <QFile>

#include <unistd.h>

#include <UserInterface/mainwindow.h>
#include <Assets/download.h>
#include <Archives/zippackage.h>

GitHubAssetManager::GitHubAssetManager(GitHubProject project, MainWindow *parent, bool always_use_latest)
: AssetManagerBase(project.install_directory() + ((project.install_directory().endsWith("/")) ? "" : "/"), parent, always_use_latest)
, project_(project)
, active_download_(nullptr)
, active_archive_(nullptr)
{
    qDebug() << "initiating asset manager for asset: " << project.project_name();

    for(auto dep : project.project_dependencies())
        qDebug() << "--- " << ((dep.is_required()) ? "required" : "optional") << " dependency " << dep.package_name();
}

GitHubAssetManager::~GitHubAssetManager()
{
    QObject::disconnect(&network_, &QNetworkAccessManager::finished, nullptr, nullptr);
}

QString GitHubAssetManager::generate_installation_name(QString tag)
{
    return (project_.asset_name() + tag);
}

void GitHubAssetManager::request_asset_ids()
{
    qDebug() << "ASSET MANAGER: Requesting assets for --> " << project_.user_name() << "/" << project_.project_name();
    connect(&network_, &QNetworkAccessManager::finished, this, &GitHubAssetManager::on_assets_received, Qt::QueuedConnection);
    QNetworkRequest request(QUrl("https://api.github.com/repos/" + project_.user_name() + "/" + project_.project_name() + "/releases"));
    request.setRawHeader("Authorization", QString("token " + project_.access_token()).toStdString().c_str());

    network_.get(request);
}

void GitHubAssetManager::check_for_updates()
{
    qDebug() << "ASSET MANAGER: Checking for updates to --> " << project_.user_name() << "/" << project_.project_name();
    connect(&network_, &QNetworkAccessManager::finished, this, &GitHubAssetManager::on_latest_received, Qt::QueuedConnection);
    QNetworkRequest request(QUrl("https://api.github.com/repos/" + project_.user_name() + "/" + project_.project_name() + "/releases/latest"));
    request.setRawHeader("Authorization", QString("token " + project_.access_token()).toStdString().c_str());

    network_.get(request);
}

void GitHubAssetManager::on_assets_received(QNetworkReply *reply)
{
    QList<ProjectTag> list;
    request_map_.clear();

    if (reply->error() == QNetworkReply::NoError)
    {
        QString reply_string = (QString) reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(reply_string.toUtf8());
        QJsonArray array;

        if (jsonResponse.isArray())
            array = jsonResponse.array();

        for (auto itr = array.begin(); itr != array.end(); itr++)
        {
            auto item = itr->toObject();
            ProjectTag *current_tag = nullptr;
            QList<Dependency> dependency_list;

            auto assets = item["assets"].toArray();
            for (auto it = assets.begin(); it != assets.end(); it++)
            {
                auto asset = it->toObject();
                for(auto dependency : project_.project_dependencies())
                    if (asset["name"].toString().compare(dependency.package_name(), Qt::CaseInsensitive) == 0)
                        dependency_list << Dependency(dependency.package_name(), asset["url"].toString(), dependency.information_filename(), dependency.is_required());

                if (asset["name"] == (project_.asset_name() + ".zip"))
                {
                    request_map_.insert(item["tag_name"].toString(), asset["url"].toString());
                    current_tag = new ProjectTag(item["tag_name"].toString());
                }
            }

            if (current_tag)
            {
                qDebug() << "ASSET MANAGER: found " << item["tag_name"];
                current_tag->add_dependency(dependency_list);
                list << *current_tag;
                delete current_tag;
            }
        }

        delete reply;

        qDebug() << "ASSET MANAGER: emitting found assets.";
        if (!list.isEmpty())
            emit provide_asset_ids(list);
    }
    else
    {
        qDebug() << "Failure: " << reply->errorString();
        delete reply;
    }
}

void GitHubAssetManager::on_latest_received(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QString reply_string = (QString) reply->readAll();
        QString tag_name;

        //parse json
        QJsonDocument jsonResponse = QJsonDocument::fromJson(reply_string.toUtf8());

        if (jsonResponse.isObject())
        {
            qDebug() << "indexing assets. ";
            auto item = jsonResponse.object();

            auto assets = item["assets"].toArray();
            for (auto it = assets.begin(); it != assets.end(); it++)
            {
                qDebug() << "ASSET MANAGER: found " << item["tag_name"];
                auto asset = it->toObject();
                if (asset["name"] == (project_.asset_name() + ".zip"))
                {
                    qDebug() << "ASSET MANAGER: found " << item["tag_name"];
                    request_map_.insert(item["tag_name"].toString(), asset["url"].toString());
                    tag_name = item["tag_name"].toString();
                }
            }
        }

        delete reply;

        if (tag_name.isEmpty())
        {
            qDebug() << "Required assets were not found on latest release.";
        }
        else
        {
            if (is_tag_installed(tag_name)) return;

            qDebug() << "ASSET MANAGER: providing tag name for latest.";
            emit provide_latest_id(tag_name);
        }
    }
    else
    {
        qDebug() << "Failure: " << reply->errorString();
        delete reply;
    }
}

QFuture<QString> GitHubAssetManager::download_asset(QString asset_id, QString url)
{
    qDebug() << "download initiated for Tag: " << asset_id << "at REQUEST URL: " << url;

    QString file_name = project_.asset_name() + asset_id + ".zip";
    QString save_to = install_directory_ + ((install_directory_.endsWith("/")) ? "" : "/") + file_name;

    active_download_ = new Download(save_to, asset_id, url, project_.access_token(), dynamic_cast<QWidget*>(this->parent()));

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

QFuture<QString> GitHubAssetManager::unzip_asset(QString tag)
{
    active_archive_ = new ZipPackage(install_directory_, project_.asset_name(), tag, ".zip");

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
    auto command = QString(directory_name + "/" + project_.asset_name() + ".app");
    qDebug() << "Use: " << command;

    auto link_name = QString(install_directory_ + project_.asset_name());
    if (!link_name.isEmpty())
    {
        QFile file(link_name);
        file.remove();
    }
    symlink(command.toStdString().c_str(), link_name.toStdString().c_str());

    auto desktop_link_name = QString("/Users/" + QString(std::getenv("USER")) + "/Desktop/" + project_.asset_name());
    if (!desktop_link_name.isEmpty())
    {
        QFile file(desktop_link_name);
        file.remove();
    }
    symlink(command.toStdString().c_str(), desktop_link_name.toStdString().c_str());
}