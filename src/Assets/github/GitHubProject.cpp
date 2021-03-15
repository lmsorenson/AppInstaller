#include "GitHubProject.h"

void GitHubProject::validate(std::function<void(QString parameter_name)> callback)
{
    if (user_name_.trimmed().isEmpty())
        callback("gh_user_name");

    if (project_name_.trimmed().isEmpty())
        callback("gh_project_name");

    if (asset_name_.trimmed().isEmpty())
        callback("gh_asset_name");

    if (access_token_.trimmed().isEmpty())
        callback("gh_deploy_key");

    if (install_directory_.trimmed().isEmpty())
        callback("install_directory");
}