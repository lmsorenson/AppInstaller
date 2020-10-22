#include "download.h"
#include "../progressdialog.h"

#include <QWidget>

#define CURL_STATICLIB
#include <curl/curl.h>
#include <sstream>
#include <curl/easy.h>
#include <iostream>
#include <fstream>

#include <QtConcurrent>

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

#include <QDebug>

struct myprogress {
  TIMETYPE lastruntime;
  CURL *curl;
};

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int download::progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow)
{
    if (dltotal != 0)
    {
        long progress = (dlnow / dltotal);
        qDebug() << "Now: " << dlnow << "total: " << dltotal << "Progress: " << progress;

    }

    return 0;
}

download::download(QObject *parent)
    : QObject(parent)
{
    qDebug() << "Initiating a file download...";
    curl_ = curl_easy_init();

    auto widget = dynamic_cast<QWidget*>(parent);

    if (widget != nullptr)
    {
        qDebug() << "cast succeeded";
    }
    else
    {
        qDebug() << "cast failed";
    }

}

download::download(const download &download)
{
    curl_ = download.curl_;
}

int32_t download::run(std::string tag, std::string filename, std::string url, std::string authorization_token)
{

    CURLcode res;
    char out_filename[FILENAME_MAX];
    sprintf(out_filename, "/Users/Shared/AgCab/%s%s.zip", filename.c_str(), tag.c_str());
    qDebug() << out_filename;
    struct myprogress prog;

    if ( curl_ != nullptr )
    {
        FILE * fp = fopen(out_filename, "wp");

        prog.lastruntime = 0;
        prog.curl = curl_;

        if (fp)
        {
            qDebug() << "Sure could write the file." << url.c_str();

            struct curl_slist *list = NULL;

            std::string authorization_header = std::string("Authorization: token ").append(authorization_token);
            list = curl_slist_append(list, authorization_header.c_str());
            list = curl_slist_append(list, "Accept: application/octet-stream");
            list = curl_slist_append(list, "Connection: keep-alive");
            list = curl_slist_append(list, "Accept-Encoding: gzip, deflate, br");

            curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, list);
            curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl_, CURLOPT_XFERINFOFUNCTION, &download::progress_callback);
            curl_easy_setopt(curl_, CURLOPT_XFERINFODATA, &prog);
            curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl_, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl_, CURLOPT_USERAGENT, "curl/7.42.0");
            curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);


            emit make_progress(60);
            res = curl_easy_perform(curl_);
        }
        else
        {
            qDebug() << "Could not write file";
        }
    }

    return 0;
}

void download::on_interval()
{
    qDebug("on interval");
}


download::~download()
{
    qDebug() << "File download complete.";
    curl_easy_cleanup(curl_);
}


