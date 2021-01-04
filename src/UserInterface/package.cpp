#include "package.h"
#include <QStringListModel>
#include <QDebug>

#include <UserInterface/notificationdialog.h>

PackageInterface::PackageInterface(QObject * parent)
: QObject(parent)
{

}

void PackageInterface::set_view(QListView * list_view)
{
    if (!list_view)
    {
        list_display_ = nullptr;
        return;
    }

    list_display_ = list_view;
    list_display_->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    list_display_->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void PackageInterface::provide_assets(QList<ProjectTag> tags)
{
    if (!tags.empty())
    {
        qDebug() << "MAIN WINDOW: Assets received.";
    }
    else
    {
        qDebug() << "MAIN WINDOW: Asset signal was empty.  No Assets to display.";
        return;
    }

    if (!list_display_)
    {
        qDebug() << "Display is null.";
        return;
    }

    QStringListModel * model = new QStringListModel(this->parent());
    QStringList tag_name_list;

    for (auto item : tags)
    {
        qDebug() << "PACKAGE INTERFACE: '" << item.tag() << "' item.";
        for (auto dependency : item.dependency_list())
        {
            qDebug() << "PACKAGE INTERFACE: '" << item.tag() << "' dependency - " << dependency.name() << " - " << dependency.url();
        }

        tag_name_list << item.tag();
    }

    tag_name_list.removeDuplicates();
    tag_name_list.sort();

    model->setStringList(tag_name_list );
    list_display_->setModel( model );

    if (list_display_->selectionModel() != nullptr)
        connect(list_display_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_selection_changed(QItemSelection)));
}

void PackageInterface::notify_latest(QString tag_name)
{
    auto notification = new  NotificationDialog(tag_name, dynamic_cast<QWidget*>(this->parent()));
    connect(notification, &NotificationDialog::yes, [this, tag_name]() { emit install(tag_name); });
    notification->exec();
}

void PackageInterface::on_install()
{
    if (!list_display_)
    {
        qDebug() << "Display is null.";
        return;
    }

    auto selectedIndex = list_display_->currentIndex();

    if (selectedIndex.isValid())
    {
        auto data = selectedIndex.data().value<QString>();

        qDebug() << "Attempting to install: " << data;

        emit install(data);
    }
}

void PackageInterface::on_use()
{
    if (!list_display_)
    {
        qDebug() << "Display is null.";
        return;
    }

    auto selectedIndex = list_display_->currentIndex();

    if (selectedIndex.isValid())
    {
        auto data = selectedIndex.data().value<QString>();

        qDebug() << "Attempting to setup: " << data;

        emit use(data);
    }
}

void PackageInterface::on_selection_changed(const QItemSelection& selection)
{
    if (selection.indexes().size() == 1)
    {
        auto model_index = selection.indexes().takeFirst();
        if (model_index.isValid())
        {
            auto data = model_index.data().value<QString>();
            qDebug() << "Selection changed to: " << data;

            emit validate_actions(data);
        }
    }
}