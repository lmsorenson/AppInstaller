#pragma once

#include <QFutureWatcher>
#include <Assets/assetmanagerbase.h>
#include "GitHubProject.h"




class GitHubAssetManager : public AssetManagerBase
{
    Q_OBJECT
public:
    explicit GitHubAssetManager(GitHubProject project, class MainWindow *parent, bool always_use_latest = false);
    virtual ~GitHubAssetManager();

    template<class Interface> void setup(Interface * interface);

    virtual void request_asset_ids() override;
    virtual void check_for_updates() override;

    virtual QFuture<QString> download_asset(QString asset_id, QString url) override;
    virtual void download_cleanup() override;

    virtual QFuture<QString> unzip_asset(QString tag) override;
    virtual void unzip_cleanup(int result_index) override;

    virtual void use_asset(QString directory_name) override;
    virtual QString generate_installation_name(QString tag) override;

private slots:
    void on_assets_received(QNetworkReply *reply);
    void on_latest_received(QNetworkReply *reply);

private:
    GitHubProject project_;

    QNetworkAccessManager network_;
    class Download * active_download_;
    class ZipPackage * active_archive_;
};


template<class Interface>
void GitHubAssetManager::setup(Interface * package)
{
    // populate ui
    QObject::connect(this, &GitHubAssetManager::provide_asset_ids, package, &Interface::provide_assets);
    QObject::connect(this, &GitHubAssetManager::provide_latest_id, package, &Interface::notify_latest);

    // handle ui commands
    QObject::connect(package, &Interface::install, this, &GitHubAssetManager::on_install_asset);
    QObject::connect(package, &Interface::use, this, &GitHubAssetManager::on_use_asset);

    // request to validate buttons
    QObject::connect(package, &Interface::validate_actions, this, &GitHubAssetManager::check_for_install);
}