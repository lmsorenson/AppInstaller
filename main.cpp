#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <Assets/github/githubassetmanager.h>

int main(int argc, char *argv[])
{
    GitHubProject project;
    project.user_name = "lmsorenson";
    project.project_name = "AgCabLab";
    project.asset_name = "AgCab";
    project.access_token = "e1d59c7b6764186b9a4adca957a7dadead2e4ccf";

    QString install_directory = "/Users/Shared/AgCab/Game";

    QApplication a(argc, argv);
    MainWindow w(project.project_name, install_directory, project.asset_name + ".zip");
    w.show();

    auto asset_manager = new GitHubAssetManager(install_directory, project, &w);
    QObject::connect(asset_manager, &GitHubAssetManager::provide_asset_ids, &w, &MainWindow::provide_assets);
    QObject::connect(&w, &MainWindow::install, asset_manager, &GitHubAssetManager::on_install_asset);
    QObject::connect(&w, &MainWindow::use, asset_manager, &GitHubAssetManager::on_use_asset);
    QObject::connect(&w, &MainWindow::validate_actions, asset_manager, &GitHubAssetManager::check_for_install);
    QObject::connect(asset_manager, &AssetManagerBase::on_install_validated, &w, &MainWindow::on_selected_install_exists);
    asset_manager->request_asset_ids();

    return a.exec();
}
