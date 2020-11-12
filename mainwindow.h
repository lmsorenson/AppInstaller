#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QList>
#include <QtConcurrent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void provide_assets(QStringList asset_ids);

private slots:
    void on_selection_changed(const QItemSelection& selection);
    void on_install();

signals:
    void install(QString asset_id);

private:
    Ui::MainWindow *ui;

    QString title_;
};
