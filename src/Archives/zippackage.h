#pragma once

#include <QObject>
#include <QFuture>

class ZipPackage : public QObject
{
    Q_OBJECT
public:
    explicit ZipPackage(QString path, QString prefix, QString tag, QString extension);
    QStringList list();
    QFuture<QString> extract();

signals:

private:
    QString path_;
    QString filename_prefix_;
    QString tag_;
    QString extension_;
};
