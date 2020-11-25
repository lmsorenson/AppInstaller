#include "assetmanagerbase.h"

#include <QFile>
#include <QDir>

AssetManagerBase::AssetManagerBase(QString install_directory, QWidget *parent)
: QObject(parent)
, install_directory_(install_directory)
, progress_dialog_(new progressdialog(parent))
{
    connect(&install_watcher_, &QFutureWatcher<QString>::finished, this, &AssetManagerBase::download_cleanup);
    connect(&install_watcher_, &QFutureWatcher<QString>::resultReadyAt, this, &AssetManagerBase::on_unzip_asset);
    connect(&unzip_watcher_, &QFutureWatcher<QString>::resultReadyAt, this, &AssetManagerBase::unzip_cleanup);

    if (progress_dialog_)
    {
        QObject::connect(this, &AssetManagerBase::close_dialog, progress_dialog_, &progressdialog::close_dialog);
    }
}

AssetManagerBase::~AssetManagerBase()
{
    if (progress_dialog_ != nullptr)
    {
        delete progress_dialog_;
        progress_dialog_ = nullptr;
    }
}


void AssetManagerBase::on_install_asset(QString asset_id)
{
    progress_dialog_->set_title(asset_id);
    progress_dialog_->show();
    progress_dialog_->setDisabled(false);

    auto install_task = this->download_asset(asset_id, request_map_[asset_id]);

    install_watcher_.setFuture(install_task);
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

void AssetManagerBase::unzip_cleanup(int result_index)
{
    QString filename = unzip_watcher_.resultAt(result_index);
    qDebug() << "Attempting to delete archive: " << filename;

    if (!filename.isEmpty())
    {
        QFile file(filename);
        file.remove();
    }

    auto widget = dynamic_cast<QWidget*>(parent());
    if (widget != nullptr)
    {
        widget->setDisabled(false);
    }

    emit close_dialog();
    emit on_install_validated(true);
}

void AssetManagerBase::on_use_asset(QString tag)
{
    use_asset(install_directory_ + generate_installation_name(tag));
}

void AssetManagerBase::check_for_install(QString tag)
{
    QDir dir(install_directory_ + generate_installation_name(tag));

    if (dir.exists() && !dir.isEmpty())
        emit on_install_validated(true);

    else
        emit on_install_validated(false);
}