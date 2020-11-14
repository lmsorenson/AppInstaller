#pragma once

#include <QDialog>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class progressdialog; }
QT_END_NAMESPACE

class progressdialog : public QDialog
{
    Q_OBJECT

public:
    explicit progressdialog(QWidget *parent = nullptr);
    progressdialog(const progressdialog &progressdialog);
    ~progressdialog();

public:
    void set_title(QString title);

public slots:
    void add_progress(int progress);
    void close_dialog();

signals:

private:
    Ui::progressdialog *ui;
};
