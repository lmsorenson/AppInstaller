#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class progressdialog;
}

class progressdialog : public QDialog
{
    Q_OBJECT

public:
    explicit progressdialog(QWidget *parent = nullptr);
    progressdialog(const progressdialog &progressdialog);
    ~progressdialog();

public slots:
    void add_progress(int progress);

signals:

private:
    Ui::progressdialog *ui;
};

#endif // PROGRESSDIALOG_H
