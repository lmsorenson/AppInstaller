#pragma once

#include <QObject>
#include <QFuture>
#include <QNetworkReply>
#include <QFutureWatcher>

#include <UserInterface/progressdialog.h>
#include <Assets/Models/tag.h>

class AssetManagerBase : public QObject
{
    Q_OBJECT

public:
    explicit AssetManagerBase(QString install_directory, class MainWindow *parent, bool always_use_latest = false);
    virtual ~AssetManagerBase();

    virtual void request_asset_ids() = 0;
    virtual void check_for_updates() = 0;
    virtual QFuture<QString> download_asset(QString asset_id, QString url) = 0;
    virtual QFuture<QString> unzip_asset(QString tag) = 0;
    virtual void use_asset(QString filename) = 0;
    virtual QString generate_installation_name(QString tag) = 0;
    bool is_tag_installed(QString tag);

public slots:
    void on_install_asset(QString asset_id);
    void on_use_asset(QString tag);
    void check_for_install(QString tag);

protected slots:
    virtual void download_cleanup() = 0;
    void on_unzip_asset(int result_index);
    virtual void unzip_cleanup(int result_index);

signals:
    void provide_asset_ids(QList<ProjectTag> string_list);
    void provide_latest_id(QString tag_name);
    void on_install_validated(bool is_installed);
    void close_dialog();

protected:
    QString install_directory_;
    QMap<QString, QString> request_map_;
    QFutureWatcher<QString> install_watcher_;
    QFutureWatcher<QString> unzip_watcher_;
    progressdialog * progress_dialog_;

private:
    bool always_use_latest_;
};
