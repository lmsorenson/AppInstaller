#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QList>
#include <QtConcurrent>
#include <src/UserInterface/package.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString project_name, QString install_directory, QString asset_name, QWidget *parent = nullptr);
    ~MainWindow();

    const PackageInterface * const get_interface();
    const PackageInterface * const get_self_interface();

public slots:
    void on_selected_install_exists(bool install_exists);

private slots:
    void to_self_update_screen();
    void to_install_screen();

private:
    Ui::MainWindow *ui;

    PackageInterface *interface_;
    PackageInterface *self_;
    QString title_;
};
