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

    // search the repository for assets.
    virtual void request_asset_ids() = 0;
    virtual void check_for_updates() = 0;

    // download an asset by its asset_id and url
    virtual QFuture<QString> download_asset(QString asset_id, QString url) = 0;

    // unzip a tagged asset.
    virtual QFuture<QString> unzip_asset(QString tag) = 0;

    // create a link to the asset.
    virtual void use_asset(QString filename) = 0;

    // derives the installation name from its tag.
    virtual QString generate_installation_name(QString tag) = 0;

    // checks if a tag is installed.
    bool is_tag_installed(QString tag);

public slots:
    // called when the UI wants to install an asset.
    void on_install_asset(QString asset_id);

    // called when the UI wants to select a new tag for use.
    void on_use_asset(QString tag);

    // Called when the UI wants to verify whether or not a tag is installed.
    void check_for_install(QString tag);

protected slots:
    // Called internally when the download has completed
    virtual void download_cleanup() = 0;

    // Called internally after a download has completed.  Unzips a given asset.
    void on_unzip_asset(int result_index);

    // Called internally after an unzip operation has completed.
    virtual void unzip_cleanup(int result_index);

    // Called internally as a download step.
    // Initiates a download for each named asset that was found on the repository.
    virtual void begin_install_dependencies(QString tag) = 0;
    virtual void install_current_dependency() = 0;

    // Called internally after all steps of the install have completed.
    void finish_install();

signals:
    // sends the validated asset ids to the UI.
    void provide_asset_ids(QList<ProjectTag> string_list);
    void provide_latest_id(QString tag_name);

    // sends confirmation to the UI to indicate whether or not an asset is installed.
    void on_install_validated(bool is_installed);

    // indicates when an asset was installed.
    void asset_installed(QString tag);

    // indicates to all listeners that the install of a tag was completed.
    void install_finished(QString tag);

    // signals when to begin installing dependencies.
    void dependency_install_ready();

    // signals when all dependencies have been installed.
    void dependencies_installed();

    // A signal to close the download status modal.
    void close_dialog();

    // Sends a signal to a UI to prompt the user for a missing config parameter.
    void request_config_parameter(QString parameter_name);

protected:
    QString install_directory_;
    QMap<QString, ProjectTag*> request_map_;
    QFutureWatcher<QString> asset_install_watcher_;
    QFutureWatcher<QString> asset_unzip_watcher_;
    progressdialog * progress_dialog_;

private:
    bool always_use_latest_;
};
