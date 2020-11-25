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
    MainWindow(QString project_name, QString install_directory, QString asset_name, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void provide_assets(QStringList asset_ids);
    void on_selected_install_exists(bool install_exists);

private slots:
    void on_selection_changed(const QItemSelection& selection);
    void on_install();
    void on_use();
    void to_self_update_screen();
    void to_install_screen();

signals:
    void install(QString asset_id);
    void use(QString tag);
    void validate_actions(QString asset_id);

private:
    Ui::MainWindow *ui;

    QString title_;
};
