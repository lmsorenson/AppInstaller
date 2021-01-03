#include <QWidget>

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory>

#include <QtConcurrent>
#include <QDebug>

#include "Assets/download.h"





size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
int progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow);

Download::Download(QString save_to, QString tag, QString url, QString authorization_token, QWidget *parent)
    : QObject(parent)
    , curl_(curl_easy_init())
    , file_(nullptr)
    , save_to_(save_to)
    , tag_(tag)
    , url_(url)
    , auth_token_(authorization_token)
    , timer_(std::make_shared<QTimer>(this))
{
    QObject::connect(timer_.get(), &QTimer::timeout, this, &Download::on_interval);
    QObject::connect(&download_watcher_, &QFutureWatcher<QString>::finished, this, &Download::on_download_finished);

    if (parent)
        parent->setDisabled(true);

    if (timer_)
        timer_->start(5);
}


QFuture<QString> Download::run()
{
    CURLcode res;

    if ( curl_ != nullptr )
    {
        file_ = fopen(save_to_.toStdString().c_str(), "wp");

        progress_.lastruntime = 0;
        progress_.curl = curl_;
        progress_.now = 0;
        progress_.total = 0;

        if (file_)
        {
            struct curl_slist *list = NULL;
            QString authorization_header = QString("Authorization: token ").append(auth_token_);
            list = curl_slist_append(list, authorization_header.toStdString().c_str());
            list = curl_slist_append(list, "Accept: application/octet-stream");
            list = curl_slist_append(list, "Connection: keep-alive");
            list = curl_slist_append(list, "Accept-Encoding: gzip, deflate, br");

            curl_easy_setopt(curl_, CURLOPT_URL, url_.toStdString().c_str());
            curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, list);
            curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl_, CURLOPT_XFERINFOFUNCTION, progress_callback);
            curl_easy_setopt(curl_, CURLOPT_XFERINFODATA, &progress_);
            curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl_, CURLOPT_WRITEDATA, file_);
            curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl_, CURLOPT_USERAGENT, "curl/7.42.0");
            curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);


            auto task = QtConcurrent::run([&](){
                res = curl_easy_perform(curl_);
                return tag_;
            });

            download_watcher_.setFuture(task);

            return task;
        }
        else
        {
            qDebug() << "Could not write file";
        }
    }

    return QFuture<QString>();
}

void Download::on_interval()
{
    double percentage = progress_.now / progress_.total;
    emit make_progress(percentage * 100);
}

void Download::on_download_finished()
{
    qDebug() << "Download finished. ";
    emit make_progress(100);

    fclose(file_);
    file_ = nullptr;

    if (timer_ != nullptr)
        timer_->stop();
}

Download::~Download()
{
    qDebug() << "File download complete.";
    curl_easy_cleanup(curl_);
}


size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow)
{
    struct myprogress *myp = (struct myprogress *)clientp;
      CURL *curl = myp->curl;
      TIMETYPE curtime = 0;

      curl_easy_getinfo(curl, TIMEOPT, &curtime);

      if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL)
      {
            myp->lastruntime = curtime;
      }

      myp->now = dlnow;
      myp->total = dltotal;

      return 0;
}
