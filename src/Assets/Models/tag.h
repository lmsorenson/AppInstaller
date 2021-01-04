#pragma once
#include <QString>
#include <QList>

class Dependency
{
public:
    Dependency(QString name, QString package_url_, QString info_file_name, bool is_required);
    ~Dependency() = default;

    QString name();
    QString url();

private:
    QString package_name_;
    QString package_url_;
    QString information_filename_;
    bool is_required_;
};

class ProjectTag
{
public:
    ProjectTag(QString name, QString url);
    ~ProjectTag() = default;

    QString tag() const;
    QString url() const;
    QList<Dependency> dependency_list() const;

    void add_dependency(QList<Dependency> dependency);

private:
    QString tag_name_;
    QString tag_download_url_;
    QList<Dependency> dependency_assets_;
};


