#include <QString>
#include <QFile>
#include <QList>
#include <QJsonDocument>
#include <QJsonObject>
#include <Assets/github/githubassetmanager.h>

GitHubProject LoadProject(QString name, int &err)
{
    QString val;
    QFile file;
    file.setFileName("/Users/Shared/AgCab/config.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
    if(!document.isObject())
    {
        err = -1;
        return GitHubProject();
    }

    auto object = document.object();

    auto project_json = object[name];
    if (!project_json.isObject())
    {
        err = -1;
        return GitHubProject();
    }

    auto project_object = project_json.toObject();

    GitHubProject project = GitHubProject(
                project_object["gh_user_name"].toString(),
                project_object["gh_project_name"].toString(),
                project_object["gh_asset_name"].toString(),
                project_object["gh_deploy_key"].toString(),
                project_object["install_directory"].toString());

    qDebug() << "gh_user_name: " << project.user_name();
    qDebug() << "gh_project_name: " << project.project_name();
    qDebug() << "gh_asset_name: " << project.asset_name();
    qDebug() << "install_directory: " << project.install_directory();

    auto deps = project_object["gh_dependencies"];
    if (!deps.isArray())
    {
        err =  0;
        return project;
    }

    auto dep_array = deps.toArray();
    for(auto dependency_value : dep_array)
    {
        if (!dependency_value.isObject()) continue;
        auto dependency = dependency_value.toObject();

        // package name is required, skip this if not defined.
        if (dependency["dependency_package_name"].isUndefined()) continue;

        auto dependency_struct = GitHubProjectDependency(
                dependency["dependency_package_name"].toString(),
                dependency["dependency_information_file"].toString(),
                dependency["dependency_required"].toBool());

        qDebug() << "---- dependency_package_name: " << dependency_struct.package_name();
        qDebug() << "---- dependency_information_file: " << dependency_struct.information_filename();
        qDebug() << "---- dependency_required: " << dependency_struct.is_required();

        project.add_dependency(dependency_struct);
    }

    qDebug() << "finished reading configuration.";

    err = 0;
    return project;
}