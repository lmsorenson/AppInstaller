#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <Assets/github/githubassetmanager.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    auto asset_manager = new GitHubAssetManager(&w);

    w.show();

    QObject::connect(asset_manager, &GitHubAssetManager::provide_asset_ids, &w, &MainWindow::provide_assets);
    QObject::connect(&w, &MainWindow::install, asset_manager, &GitHubAssetManager::on_install_asset);

    asset_manager->request_asset_ids();

    return a.exec();
}
