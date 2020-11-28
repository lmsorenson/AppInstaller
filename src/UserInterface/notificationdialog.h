#pragma once

#include <QDialog>

namespace Ui {
class NotificationDialog;
}

class NotificationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NotificationDialog(QString tag_name, QWidget *parent = nullptr);
    ~NotificationDialog();

private slots:
    void on_yes();
    void close_dialog();

signals:
    void yes();

private:
    Ui::NotificationDialog *ui;
};
