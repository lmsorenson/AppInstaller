#pragma once

#include <QObject>
#include <QFuture>
#include <QNetworkReply>
#include <QFutureWatcher>
#include <src/Assets/progressdialog.h>

class AssetManagerBase : public QObject
{
    Q_OBJECT

public:
    explicit AssetManagerBase(QString install_directory, QWidget *parent);
    virtual ~AssetManagerBase();

    virtual void request_asset_ids() = 0;
    virtual QFuture<QString> download_asset(QString asset_id, QString url) = 0;
    virtual QFuture<QString> unzip_asset(QString file_name) = 0;
    virtual void use_asset(QString filename) = 0;
    virtual QString generate_installation_name(QString tag) = 0;

public slots:
    void on_install_asset(QString asset_id);
    void on_use_asset(QString tag);
    void check_for_install(QString tag);

protected slots:
    virtual void download_cleanup() = 0;
    void on_unzip_asset(int result_index);
    virtual void unzip_cleanup(int result_index);

signals:
    void on_install_validated(bool is_installed);
    void on_install_complete(QString directory_name);
    void close_dialog();

protected:
    QString install_directory_;
    QMap<QString, QString> request_map_;
    QFutureWatcher<QString> install_watcher_;
    QFutureWatcher<QString> unzip_watcher_;
    progressdialog * progress_dialog_;
};
