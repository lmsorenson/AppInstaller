#pragma once

#include <QFutureWatcher>
#include <Assets/assetmanagerbase.h>

struct GitHubProject
{
    QString user_name;
    QString project_name;
    QString asset_name;
    QString access_token;
};

class GitHubAssetManager : public AssetManagerBase
{
    Q_OBJECT
public:
    explicit GitHubAssetManager(QString install_directory, GitHubProject project, QWidget *parent);

    virtual void request_asset_ids() override;
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
