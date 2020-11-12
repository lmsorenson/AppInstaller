#pragma once

#include <QObject>
#include <QFuture>
#include <QNetworkReply>
#include <QFutureWatcher>

class AssetManagerBase : public QObject
{
    Q_OBJECT
public:
    explicit AssetManagerBase(QObject *parent);

    virtual void request_asset_ids() = 0;
    virtual QFuture<QString> download_asset(QString asset_id, QString url) = 0;
    virtual QFuture<void> unzip_asset(QString file_name) = 0;

public slots:
    void on_install_asset(QString asset_id);

private slots:
    void on_unzip_asset(int result_index);
    void on_unzip_asset_complete();

protected:
    QMap<QString, QString> request_map_;
    QFutureWatcher<QString> install_watcher_;
    QFutureWatcher<void> unzip_watcher_;
};
