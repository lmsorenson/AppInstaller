#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QMap>
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

private slots:
    void on_network_connection_made(QNetworkReply *reply);
    void on_selection_changed(const QItemSelection& selection);
    void on_install();
    void on_new_zip();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager network_;
    QMap<QString, QString> map_;
    class Download * active_download_;
    QFuture<void> future_;
    QFutureWatcher<void> watcher_;
};
