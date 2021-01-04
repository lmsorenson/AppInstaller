#include <UserInterface/mainwindow.h>
#include <QApplication>
#include <QObject>
#include <Assets/github/githubassetmanager.h>
#include <Configuration.h>

GitHubAssetManager *self_update_manager = nullptr, *project_manager = nullptr;

int main(int argc, char *argv[])
{
    qDebug() << "Initializing the installer.";

    int project_config_load_status = 0, self_update_config_load_status = 0;

    GitHubProject project = LoadProject("project", project_config_load_status);
    if (project_config_load_status != 0) { return 1; }

    //---- CREATE APPLICATION AND WINDOW ----
    qDebug() << "Creating application and window.";
    QApplication a(argc, argv);
    MainWindow w(project.project_name(), project.install_directory(), project.asset_name() + ".zip");
    w.show();

    //---- SETUP ASSET ----
    qDebug() << "Setup project manager.";
    project_manager = new GitHubAssetManager(project, &w);
    project_manager->setup(w.get_interface());
    QObject::connect(project_manager, &GitHubAssetManager::on_install_validated, &w, &MainWindow::on_selected_install_exists);
    project_manager->request_asset_ids();

    //---- SETUP SELF UPDATE ----
    GitHubProject self = LoadProject("self", self_update_config_load_status);
    if (self_update_config_load_status == 0 )
    {
        qDebug() << "Setting up self update manager. ";
        self_update_manager = new GitHubAssetManager(self, &w, true);
        self_update_manager->setup(w.get_self_interface());
        self_update_manager->check_for_updates();
    }

    auto result = a.exec();

    return result;
}