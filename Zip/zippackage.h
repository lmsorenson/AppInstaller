#pragma once

#include <QObject>

class ZipPackage : public QObject
{
    Q_OBJECT
public:
    explicit ZipPackage(QString path, QString file_name, QString extension);
    QStringList list();
    void extract();

signals:

private:
    QString path_;
    QString file_name_;
    QString extension_;
};
