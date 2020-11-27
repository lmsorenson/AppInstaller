#include "package.h"
#include <QStringListModel>
#include <QDebug>

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

void PackageInterface::provide_assets(QStringList asset_ids)
{
    if (!asset_ids.empty())
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
    QStringList list;

    for (auto item : asset_ids)
    {
        qDebug() << "PACKAGE INTERFACE: '" << item << "' item.";
        list << item;
    }

    list.removeDuplicates();
    list.sort();

    model->setStringList( list );
    list_display_->setModel( model );

    if (list_display_->selectionModel() != nullptr)
        connect(list_display_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(on_selection_changed(QItemSelection)));
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