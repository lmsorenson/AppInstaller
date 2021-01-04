#pragma once

#include <QObject>
#include <QListView>
#include <QList>
#include <Assets/Models/tag.h>

class PackageInterface : public QObject
{
    Q_OBJECT
public:
    PackageInterface(QObject * parent = nullptr);

    void set_view(QListView * list_view);

public slots:
    void provide_assets(QList<ProjectTag> tags);
    void notify_latest(QString tag_name);
    void on_install();
    void on_use();

private slots:
    void on_selection_changed(const QItemSelection& selection);

signals:
    void install(QString asset_id);
    void use(QString tag);
    void validate_actions(QString asset_id);

private:
    QListView * list_display_;
};
