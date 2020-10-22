#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "progressdialog.h"
#include "Files/download.h"
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

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager network_;
    QMap<QString, QString> map_;
    download * active_download_;
    QFuture<void> future_;

};
#endif // MAINWINDOW_H
