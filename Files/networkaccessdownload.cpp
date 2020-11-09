#include "networkaccessdownload.h"
#include <QNetworkReply>

NetworkAccessDownload::NetworkAccessDownload(QUrl url, QObject *parent)
    : QObject(parent)
    , is_finished_(false)
{
    QNetworkRequest request(url);

    request.setRawHeader("Authorization", "token e1d59c7b6764186b9a4adca957a7dadead2e4ccf");
    request.setRawHeader("Accept", "application/octet-stream");
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Accept-Encoding", "gzip, deflate, br");

    qDebug() << "Sending request. ";
    reply_ = network_.get(request);

//    QObject::connect(reply_, SIGNAL(finished), this, SLOT(on_finished));
//    QObject::connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(on_error(QNetworkReply::NetworkError)));
//    QObject::connect(reply_, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(on_finished(qint64, qint64)));
}

bool NetworkAccessDownload::is_finished()
{
    return is_finished_;
}

void NetworkAccessDownload::on_finished()
{
    qDebug() << "finished";

//    if (reply->error() == QNetworkReply::NoError)
//    {
//        QString reply_string = (QString)reply->readAll();


//        qDebug() << reply_string;


//        delete reply;
//    }
//    else
//    {
//        qDebug() << "Failure" <<reply->errorString();
//        delete reply;
//    }

    is_finished_ = true;
}

void NetworkAccessDownload::on_error(QNetworkReply::NetworkError error)
{
    qDebug() << "Error: " << error;

    is_finished_ = true;
}

void NetworkAccessDownload::on_progress(qint64 read, qint64 total)
{
    qDebug() << "progress made";
}
