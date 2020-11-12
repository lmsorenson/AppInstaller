#pragma once

#include <QFutureWatcher>
#include <Assets/assetmanagerbase.h>

class GitHubAssetManager : public AssetManagerBase
{
    Q_OBJECT
public:
    explicit GitHubAssetManager(QWidget *parent);

    virtual QFuture<QStringList> request_asset_ids() override;
    virtual QFuture<QString> download_asset(QString asset_id, QString url) override;
    virtual QFuture<void> unzip_asset(QString file_name) override;

private slots:
    void on_assets_received(QNetworkReply *reply);

signals:
    void provide_asset_ids(QStringList string_list);


private:
    QNetworkAccessManager network_;
    QString github_username_;
    QString github_project_;
    QString github_token_;
    QString github_asset_name_;
    QString install_directory_;

    class Download * active_download_;
    class ZipPackage * active_archive_;
};
