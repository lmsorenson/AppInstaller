#pragma once
#include <QObject>
#include "progressdialog.h"

#include <curl/curl.h>
#include <QFuture>

#if LIBCURL_VERSION_NUM >= 0x073d00
#define TIME_IN_US 1
#define TIMETYPE curl_off_t
#define TIMEOPT CURLINFO_TOTAL_TIME_T
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3000000
#else
#define TIMETYPE double
#define TIMEOPT CURLINFO_TOTAL_TIME
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL     3
#endif

#define STOP_DOWNLOAD_AFTER_THIS_MANY_BYTES         6000


struct myprogress {
  TIMETYPE lastruntime;
  CURL *curl;
  double now;
  double total;
};

class Download : public QObject
{
    Q_OBJECT

public:
    explicit Download(QObject *parent = nullptr);
    explicit Download(QString tag, QString filename, QString url, QString authorization_token, QObject *parent = nullptr);
    Download (const Download &Download);
    ~Download();


    Download& operator=(const Download&);

    QFuture<void> run();

public slots:
    void on_interval();

signals:
    void make_progress(int progress);
    void close_dialog();

private:
    QString url_;
    QString tag_;
    QString filename_;
    QString auth_token_;

    CURL *curl_;
    progressdialog *progress_dialog_;
    myprogress progress_;

    std::shared_ptr<QTimer> timer_;
};
