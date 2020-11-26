#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <src/Assets/github/githubassetmanager.h>
#include <src/Assets/package.h>
#include <exception>

int LoadProject(QString name, GitHubProject &project, QString &install_directory);

template<class Manager, class Interface>
void SetupManager(Manager * manager, Interface * package);

GitHubAssetManager * self_update_manager = nullptr;
GitHubAssetManager * project_manager = nullptr;

int main(int argc, char *argv[])
{
    GitHubProject project, self;
    QString project_install_directory, self_install_directory;

    if (LoadProject("project", project, project_install_directory) != 0)
        return 1;

    bool self_update_enabled = (LoadProject("self", self, self_install_directory) == 0 );

    QApplication a(argc, argv);
    MainWindow w(project.project_name, project_install_directory, project.asset_name + ".zip");
    w.show();

    self_update_manager = (self_update_enabled)
            ? new GitHubAssetManager("Installer", "ReleaseInstaller.app", self_install_directory, self, &w) : nullptr;
    project_manager = new GitHubAssetManager("AgCabLab", "AgCab.app", project_install_directory, project, &w);


    SetupManager(project_manager, w.get_interface());
    QObject::connect(project_manager, &GitHubAssetManager::on_install_validated, &w, &MainWindow::on_selected_install_exists);

    try
    {
        if (!project_manager)
        {
            qDebug() << "Asset manager is null.";
            return -1;
        }

        project_manager->request_asset_ids();
    }
    catch (std::exception ex)
    {
        qDebug() << "Failed to setup asset manager";
    }

    qDebug() << "Setup complete: launching installer.";

    auto result = a.exec();

    return result;
}

int LoadProject(QString name, GitHubProject &project, QString &install_directory)
{
    QString val;
    QFile file;
    file.setFileName("/Users/Shared/AgCab/config.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());

    if(!document.isObject())
        return -1;

    auto object = document.object();

    auto project_json = object[name];
    if (!project_json.isObject())
        return -1;

    auto project_object = project_json.toObject();

    project.user_name = project_object["gh_user_name"].toString();
    project.project_name = project_object["gh_project_name"].toString();
    project.asset_name = project_object["gh_asset_name"].toString();
    project.access_token = project_object["gh_deploy_key"].toString();
    install_directory = project_object["install_directory"].toString();

    return 0;
}

template<class Manager, class Interface>
void SetupManager(Manager * manager, Interface * package)
{
    // populate ui
    QObject::connect(project_manager, &Manager::provide_asset_ids, package, &Interface::provide_assets);

    // handle ui commands
    QObject::connect(package, &Interface::install, project_manager, &Manager::on_install_asset);
    QObject::connect(package, &Interface::use, project_manager, &Manager::on_use_asset);

    // request to validate buttons
    QObject::connect(package, &Interface::validate_actions, project_manager, &Manager::check_for_install);
}
