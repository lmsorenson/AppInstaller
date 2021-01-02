#pragma once

class GitHubProjectDependency
{
public:
    GitHubProjectDependency(QString name, QString info_file, bool is_required)
    : package_name_(name), information_filename_(info_file), is_required_(is_required) {}
    ~GitHubProjectDependency() = default;

    QString package_name() { return package_name_; }
    QString information_filename() { return information_filename_; }
    bool is_required() { return is_required_; }

private:
    QString package_name_;
    QString information_filename_;
    bool is_required_;
};

class GitHubProject
{
public:
    GitHubProject() = default;
    GitHubProject(QString user, QString project, QString asset, QString token, QString install_directory)
    : user_name_(user) , project_name_(project), asset_name_(asset), access_token_(token), install_directory_(install_directory)
    {
        project_dependencies_ = QList<GitHubProjectDependency>();
    }
    ~GitHubProject() = default;

    void add_dependency(GitHubProjectDependency dep) { project_dependencies_ << dep; }

    QString user_name() { return user_name_; };
    QString project_name() { return project_name_; };
    QList<GitHubProjectDependency> project_dependencies() { return project_dependencies_; };
    QString asset_name() { return asset_name_; };
    QString access_token() { return access_token_; };
    QString install_directory() { return install_directory_; };

private:
    QString user_name_;
    QString project_name_;
    QList<GitHubProjectDependency> project_dependencies_;
    QString asset_name_;
    QString access_token_;
    QString install_directory_;
};