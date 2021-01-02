#include "Tag.h"

ProjectTag::ProjectTag(QString name) : tag_name_(name) {}

void ProjectTag::add_dependency(Dependency dependency) { dependency_assets_ << dependency; }

QString ProjectTag::tag() const { return tag_name_; }
QList<Dependency> ProjectTag::dependency_list() const { return dependency_assets_; }

Dependency::Dependency(QString name, QString info_file_name, bool is_required)
: package_name_(name)
, information_filename_(info_file_name)
, is_required_(is_required)
{}

QString Dependency::name() { return package_name_; }