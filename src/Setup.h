#pragma once
#include <QObject>

template<class Manager, class Interface>
void SetupManager(Manager * manager, Interface * package)
{
    // populate ui
    QObject::connect(manager, &Manager::provide_asset_ids, package, &Interface::provide_assets);

    // handle ui commands
    QObject::connect(package, &Interface::install, manager, &Manager::on_install_asset);
    QObject::connect(package, &Interface::use, manager, &Manager::on_use_asset);

    // request to validate buttons
    QObject::connect(package, &Interface::validate_actions, manager, &Manager::check_for_install);
}


int LoadProject(QString name, GitHubProject &project, QString &install_directory);