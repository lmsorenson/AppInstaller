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

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    auto asset_manager = new GitHubAssetManager("/Users/Shared/AgCab", project, &w);
    QObject::connect(asset_manager, &GitHubAssetManager::provide_asset_ids, &w, &MainWindow::provide_assets);
    QObject::connect(&w, &MainWindow::install, asset_manager, &GitHubAssetManager::on_install_asset);
    asset_manager->request_asset_ids();

    return a.exec();
}
