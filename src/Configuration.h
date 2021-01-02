#include <QString>
#include <QFile>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <Assets/github/githubassetmanager.h>

int LoadProject(QString name, GitHubProject &project)
{
    QString val;
    QFile file;
    file.setFileName("/Users/Shared/AgCab/config.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    qDebug() << val;

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    if(!document.isObject()) return -1;

    auto object = document.object();

    auto project_json = object[name];
    if (!project_json.isObject()) return -1;

    auto project_object = project_json.toObject();

    project.user_name = project_object["gh_user_name"].toString();
    qDebug() << "gh_user_name: " << project.user_name;
    project.project_name = project_object["gh_project_name"].toString();
    qDebug() << "gh_project_name: " << project.project_name;
    project.project_dependencies = QList<GitHubProjectDependency>();
    project.asset_name = project_object["gh_asset_name"].toString();
    qDebug() << "gh_asset_name: " << project.asset_name;
    project.access_token = project_object["gh_deploy_key"].toString();
    project.install_directory = project_object["install_directory"].toString();
    qDebug() << "install_directory: " << project.install_directory;

    qDebug() << "gh_dependencies: ";
    qDebug() << "dependencies: " << project_object["gh_dependencies"].toString();

    auto deps = project_object["gh_dependencies"];
    if (!deps.isArray()) return 0;

    auto dep_array = deps.toArray();
    for(auto dependency_value : dep_array)
    {
        if (!dependency_value.isObject()) continue;
        auto dependency = dependency_value.toObject();

        // package name is required, skip this if not defined.
        if (dependency["dependency_package_name"].isUndefined()) continue;

        auto dependency_struct = GitHubProjectDependency();
        dependency_struct.package_name = dependency["dependency_package_name"].toString();
        qDebug() << "---- dependency_package_name: " << dependency_struct.package_name;
        dependency_struct.information_filename = dependency["dependency_information_file"].toString();
        qDebug() << "---- dependency_information_file: " << dependency_struct.information_filename;
        dependency_struct.package_name = dependency["dependency_required"].toBool();
        qDebug() << "---- dependency_required: " << dependency_struct.is_required;
        project.project_dependencies.append(dependency_struct);
    }

    qDebug() << "finished reading configuration.";

    return 0;
}