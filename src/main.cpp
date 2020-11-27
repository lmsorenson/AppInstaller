#include <src/UserInterface/mainwindow.h>
#include <QApplication>
#include <QObject>
#include <src/Assets/github/githubassetmanager.h>
#include <src/Configuration.h>


int main(int argc, char *argv[])
{
    std::shared_ptr<GitHubAssetManager> self_update_manager = nullptr, project_manager = nullptr;
    GitHubProject project, self;

    if (LoadProject("project", project) != 0) { return 1; }

    //---- CREATE APPLICATION AND WINDOW ----
    QApplication a(argc, argv);
    MainWindow w(project.project_name, project.install_directory, project.asset_name + ".zip");
    w.show();

    //---- SETUP ASSET ----
    project_manager = std::make_shared<GitHubAssetManager>("AgCabLab", "AgCab.app", project, &w);
    project_manager->setup(w.get_interface());
    QObject::connect(project_manager.get(), &GitHubAssetManager::on_install_validated, &w, &MainWindow::on_selected_install_exists);
    project_manager->request_asset_ids();

    //---- SETUP SELF UPDATE ----
    if (LoadProject("self", self) == 0 )
    {
        self_update_manager = std::make_shared<GitHubAssetManager>("Installer", "ReleaseInstaller.app", self, &w);
        self_update_manager->setup(w.get_self_interface());
        self_update_manager->request_asset_ids();
    }

    return a.exec();
}