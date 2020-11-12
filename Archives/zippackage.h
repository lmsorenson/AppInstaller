#pragma once

#include <QObject>
#include <QFuture>

class ZipPackage : public QObject
{
    Q_OBJECT
public:
    explicit ZipPackage(QString path, QString file_name, QString extension);
    QStringList list();
    QFuture<void> extract();

signals:

private:
    QString path_;
    QString file_name_;
    QString extension_;
};
