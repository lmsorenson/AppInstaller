#include "mainwindow.h"
#include <QApplication>
#include <QObject>
#include <Assets/github/githubassetmanager.h>
#include <exception>

void LoadProject(GitHubProject &project, QString &install_directory);

int main(int argc, char *argv[])
{

    GitHubProject project;
    QString install_directory;

    LoadProject(project, install_directory);

    QApplication a(argc, argv);
    MainWindow w(project.project_name, install_directory, project.asset_name + ".zip");
    w.show();

    try
    {
        auto asset_manager = new GitHubAssetManager("AgCabLab", "AgCab.app", install_directory, project, &w);

        if (!asset_manager)
        {
            qDebug() << "Asset manager is null.";
            return -1;
        }

        QObject::connect(asset_manager, &GitHubAssetManager::provide_asset_ids, &w, &MainWindow::provide_assets);
        QObject::connect(&w, &MainWindow::install, asset_manager, &GitHubAssetManager::on_install_asset);
        QObject::connect(&w, &MainWindow::use, asset_manager, &GitHubAssetManager::on_use_asset);
        QObject::connect(&w, &MainWindow::validate_actions, asset_manager, &GitHubAssetManager::check_for_install);
        QObject::connect(asset_manager, &AssetManagerBase::on_install_validated, &w, &MainWindow::on_selected_install_exists);

        asset_manager->request_asset_ids();
    }
    catch (std::exception ex)
    {
        qDebug() << "Failed to setup asset manager";
    }

    qDebug() << "Setup complete: launching installer.";
    return a.exec();
}

void LoadProject(GitHubProject &project, QString &install_directory)
{
    QString val;
    QFile file;
    file.setFileName("/Users/Shared/AgCab/config.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());

    if(document.isObject())
    {
        auto object = document.object();

        project.user_name = object["gh_user_name"].toString();
        project.project_name = object["gh_project_name"].toString();
        project.asset_name = object["gh_asset_name"].toString();
        project.access_token = object["gh_access_token"].toString();
        install_directory = object["install_directory"].toString();
    }
}
