#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

int LoadProject(QString name, GitHubProject &project, QString &install_directory)
{
    QString val;
    QFile file;
    file.setFileName("/Users/Shared/AgCab/config.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());

    if(!document.isObject())
        return -1;

    auto object = document.object();

    auto project_json = object[name];
    if (!project_json.isObject())
        return -1;

    auto project_object = project_json.toObject();

    project.user_name = project_object["gh_user_name"].toString();
    project.project_name = project_object["gh_project_name"].toString();
    project.asset_name = project_object["gh_asset_name"].toString();
    project.access_token = project_object["gh_deploy_key"].toString();
    install_directory = project_object["install_directory"].toString();

    return 0;
}