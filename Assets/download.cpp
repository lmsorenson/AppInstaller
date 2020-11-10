#include <QWidget>

#define CURL_STATICLIB
#include <curl/curl.h>
#include <curl/easy.h>
#include <memory>

#include <QtConcurrent>
#include <QDebug>

#include "Assets/download.h"
#include "Assets/progressdialog.h"





size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
int progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow);

Download& Download::operator=(const Download& d)
{
    this->url_ = d.url_;
    this->tag_ = d.tag_;
    this->filename_ = d.filename_;
    this->auth_token_ = d.auth_token_;
    this->progress_dialog_ = d.progress_dialog_;

    this->curl_ = curl_easy_init();

    return *this;
}

Download::Download(QObject *parent)
    : QObject(parent)
{
}

Download::Download(QString tag, QString filename, QString url, QString authorization_token, QObject *parent)
    : QObject(parent)
{
    qDebug() << "Initiating a file download...";
    curl_ = curl_easy_init();

    auto widget = dynamic_cast<QWidget*>(parent);

    if (widget != nullptr)
    {
        qDebug() << "cast succeeded";
        progress_dialog_ = new progressdialog(widget);
        QObject::connect(this, &Download::make_progress, progress_dialog_, &progressdialog::add_progress);
        progress_dialog_->setWindowTitle(filename + tag + " download");
        widget->setDisabled(true);
        progress_dialog_->show();
        progress_dialog_->setDisabled(false);
    }
    else
    {
        qDebug() << "cast failed";
    }

    timer_ = std::make_shared<QTimer>(this);
    connect(timer_.get(), &QTimer::timeout, this, &Download::on_interval);
    timer_->start(5);

    tag_ = tag;
    filename_ = filename;
    url_ = url;
    auth_token_ = authorization_token;
}

Download::Download(const Download &download)
: QObject(download.parent())
{
    auto widget = dynamic_cast<QWidget*>(download.parent());

    if (widget != nullptr)
    {
        qDebug() << "cast succeeded";
        progress_dialog_ = new progressdialog(widget);
        QObject::connect(this, &Download::make_progress, progress_dialog_, &progressdialog::add_progress);
        progress_dialog_->show();
    }
    else
    {
        qDebug() << "cast failed";
    }

    curl_ = download.curl_;
    url_ = download.url_;
    tag_ = download.tag_;
    filename_ = download.filename_;
    auth_token_ = download.auth_token_;
}

QFuture<void> Download::run()
{
    CURLcode res;
    char out_filename[FILENAME_MAX];
    sprintf(out_filename, "/Users/Shared/AgCab/%s%s.zip", filename_.toStdString().c_str(), tag_.toStdString().c_str());
    qDebug() << out_filename;

    if ( curl_ != nullptr )
    {
        FILE * fp = fopen(out_filename, "wp");

        progress_.lastruntime = 0;
        progress_.curl = curl_;
        progress_.now = 0;
        progress_.total = 0;

        if (fp)
        {
            qDebug() << "Sure could write the file." << url_;

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
            curl_easy_setopt(curl_, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl_, CURLOPT_USERAGENT, "curl/7.42.0");
            curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);

            return QtConcurrent::run([&](){
                res = curl_easy_perform(curl_);
            });
        }
        else
        {
            qDebug() << "Could not write file";
        }
    }

    return QFuture<void>();
}

void Download::on_interval()
{
    double percentage = progress_.now / progress_.total;
    qDebug() << "percent: " << percentage;

    emit make_progress(percentage * 100);
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

    #ifdef TIME_IN_US
        fprintf(stderr, "TOTAL TIME: %" CURL_FORMAT_CURL_OFF_T ".%06ld\r\n",
                (curtime / 1000000), (long)(curtime % 1000000));
    #else
        fprintf(stderr, "TOTAL TIME: %f \r\n", curtime);
    #endif
      }

      myp->now = dlnow;
      myp->total = dltotal;
//      fprintf(stderr, "UP: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
//              "  DOWN: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
//              "\r\n",
//              ulnow, ultotal, dlnow, dltotal);

      return 0;
}
