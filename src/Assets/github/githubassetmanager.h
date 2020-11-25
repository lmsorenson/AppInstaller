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
    explicit GitHubAssetManager(QString asset_name, QString executable_name, QString install_directory, GitHubProject project, QWidget *parent);
    virtual ~GitHubAssetManager();

    virtual void request_asset_ids() override;

    virtual QFuture<QString> download_asset(QString asset_id, QString url) override;
    virtual void download_cleanup() override;

    virtual QFuture<QString> unzip_asset(QString file_name) override;
    virtual void unzip_cleanup(int result_index) override;

    virtual void use_asset(QString directory_name) override;
    virtual QString generate_installation_name(QString tag) override;

private slots:
    void on_assets_received(QNetworkReply *reply);

signals:
    void provide_asset_ids(QStringList string_list);


private:
    // installed asset names.
    QString asset_name_;
    QString executable_name_;

    // remote asset names.
    QString github_username_;
    QString github_project_;
    QString github_token_;
    QString github_required_asset_name_;

    QNetworkAccessManager network_;
    class Download * active_download_;
    class ZipPackage * active_archive_;
};
