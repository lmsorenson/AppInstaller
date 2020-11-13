#include "assetmanagerbase.h"

#include <QFile>
#include <QDir>

AssetManagerBase::AssetManagerBase(QString install_directory, QObject *parent)
: QObject(parent)
, install_directory_(install_directory)
{
    connect(&install_watcher_, &QFutureWatcher<QString>::resultReadyAt, this, &AssetManagerBase::on_unzip_asset);
    connect(&unzip_watcher_, &QFutureWatcher<QString>::resultReadyAt, this, &AssetManagerBase::on_unzip_asset_complete);
}


void AssetManagerBase::on_install_asset(QString asset_id)
{
    auto install_task = this->download_asset(asset_id, request_map_[asset_id]);

    install_watcher_.setFuture(install_task);
}

void AssetManagerBase::on_use_asset(QString tag)
{
    use_asset(install_directory_ + generate_installation_name(tag));
}

void AssetManagerBase::on_unzip_asset(int result_index)
{
    QString filename = install_watcher_.resultAt(result_index);

    if (!filename.isEmpty())
    {
        auto unzip_task = this->unzip_asset(filename);

        unzip_watcher_.setFuture(unzip_task);
    }
}

void AssetManagerBase::on_unzip_asset_complete(int result_index)
{
    qDebug() << "unzip complete";

    QString filename = unzip_watcher_.resultAt(result_index);

    if (!filename.isEmpty())
    {
        QFile file(filename);
        file.remove();
    }
}

void AssetManagerBase::check_for_install(QString tag)
{
    QDir dir(install_directory_ + generate_installation_name(tag));

    if (dir.exists() && !dir.isEmpty())
        emit on_install_validated(true);

    else
        emit on_install_validated(false);
}