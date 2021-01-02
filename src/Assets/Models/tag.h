#pragma once
#include <QString>
#include <QList>

class Dependency
{
public:
    Dependency(QString name, QString info_file_name, bool is_required);
    ~Dependency() = default;

    QString name();

private:
    QString package_name_;
    QString information_filename_;
    bool is_required_;
};

class ProjectTag
{
public:
    ProjectTag(QString name);
    ~ProjectTag() = default;

    QString tag() const;
    QList<Dependency> dependency_list() const;

    void add_dependency(Dependency dependency);

private:
    QString tag_name_;
    QList<Dependency> dependency_assets_;
};


