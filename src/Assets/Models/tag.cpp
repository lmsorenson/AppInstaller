#include "Tag.h"

ProjectTag::ProjectTag(QString name) : tag_name_(name) {}

void ProjectTag::add_dependency(QList<Dependency> dependencies) { dependency_assets_ << dependencies; }

QString ProjectTag::tag() const { return tag_name_; }
QList<Dependency> ProjectTag::dependency_list() const { return dependency_assets_; }

Dependency::Dependency(QString name, QString package_url, QString info_file_name, bool is_required)
: package_name_(name)
, package_url_(package_url)
, information_filename_(info_file_name)
, is_required_(is_required)
{}

QString Dependency::name() { return package_name_; }
QString Dependency::url() { return package_url_; }