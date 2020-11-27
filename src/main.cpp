#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <src/Assets/github/githubassetmanager.h>
#include <src/Setup.h>
#include <src/Configuration.h>


int main(int argc, char *argv[])
{
    GitHubAssetManager * self_update_manager = nullptr;
    GitHubAssetManager * project_manager = nullptr;

    //---- READ CONFIGURATION ----
    GitHubProject project, self;
    QString project_install_directory, self_install_directory;

    if (LoadProject("project", project, project_install_directory) != 0)
        return 1;

    bool self_update_enabled = (LoadProject("self", self, self_install_directory) == 0 );

    //---- CREATE APPLICATION AND WINDOW ----
    QApplication a(argc, argv);
    MainWindow w(project.project_name, project_install_directory, project.asset_name + ".zip");
    w.show();

    //---- CREATE AND SETUP MANAGERS FOR REQUIRED ASSETS ----
    project_manager = new GitHubAssetManager("AgCabLab", "AgCab.app", project_install_directory, project, &w);
    SetupManager(project_manager, w.get_interface());

    if (self_update_enabled)
    {
        self_update_manager = new GitHubAssetManager("Installer", "ReleaseInstaller.app", self_install_directory, self, &w);
        SetupManager(self_update_manager, w.get_self_interface());
    }
    QObject::connect(project_manager, &GitHubAssetManager::on_install_validated, &w, &MainWindow::on_selected_install_exists);

    project_manager->request_asset_ids();
    self_update_manager->request_asset_ids();

    return a.exec();
}
