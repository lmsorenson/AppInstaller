#ifndef NETWORKACCESSDOWNLOAD_H
#define NETWORKACCESSDOWNLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class NetworkAccessDownload : public QObject
{
    Q_OBJECT

public:
    explicit NetworkAccessDownload(QUrl url, QObject *parent = nullptr);

    bool is_finished();

    void on_finished();
    void on_error(QNetworkReply::NetworkError error);
    void on_progress(qint64 read, qint64 total);

private:
    QNetworkAccessManager network_;
    QNetworkReply * reply_;
    bool is_finished_;
};

#endif // NETWORKACCESSDOWNLOAD_H
