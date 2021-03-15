#pragma once

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QList>
#include <QtConcurrent>
#include <UserInterface/package.h>

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

    void prompt_for_parameter(QString parameter_name);

public slots:
    void on_selected_install_exists(bool install_exists);

private:
    Ui::MainWindow *ui;

    PackageInterface *interface_;
    PackageInterface *self_;
    QString title_;
};
