#include <Assets/Models/tag.h>

ProjectTag::ProjectTag(QString name, QString url) : tag_name_(name), tag_download_url_(url) {}

void ProjectTag::add_dependency(QList<Dependency> dependencies) { dependency_assets_ << dependencies; }

QString ProjectTag::tag() const { return tag_name_; }
QString ProjectTag::url() const { return tag_download_url_; }
QList<Dependency> ProjectTag::dependency_list() const { return dependency_assets_; }

Dependency::Dependency(QString name, QString package_url, QString info_file_name, bool is_required)
: package_name_(name)
, package_url_(package_url)
, information_filename_(info_file_name)
, is_required_(is_required)
{}

QString Dependency::name() { return package_name_; }
QString Dependency::url() { return package_url_; }