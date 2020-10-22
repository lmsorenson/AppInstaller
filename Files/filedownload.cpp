#include "filedownload.h"

#define CURL_STATICLIB
#include <curl/curl.h>
#include <sstream>
#include <curl/easy.h>
#include <iostream>
#include <fstream>

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

size_t write_data2(void *ptr, size_t size, size_t nmemb, FILE *stream)
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

      /* under certain circumstances it may be desirable for certain functionality
         to only run every N seconds, in order to do this the transaction time can
         be used */
      if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
        myp->lastruntime = curtime;
    #ifdef TIME_IN_US
        fprintf(stderr, "TOTAL TIME: %" CURL_FORMAT_CURL_OFF_T ".%06ld\r\n",
                (curtime / 1000000), (long)(curtime % 1000000));
    #else
        fprintf(stderr, "TOTAL TIME: %f \r\n", curtime);
    #endif
      }

      fprintf(stderr, "UP: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
              "  DOWN: %" CURL_FORMAT_CURL_OFF_T " of %" CURL_FORMAT_CURL_OFF_T
              "\r\n",
              ulnow, ultotal, dlnow, dltotal);


      return 0;
}

FileDownload::FileDownload()
{
    qDebug() << "Initiating a file download...";
    curl_ = curl_easy_init();
}

int32_t FileDownload::download(std::string tag, std::string filename, std::string url, std::string authorization_token)
{

    CURLcode res;
    char out_filename[FILENAME_MAX];
    sprintf(out_filename, "/Users/Shared/AgCab/%s%s.zip", filename.c_str(), tag.c_str());
    qDebug() << out_filename;
    struct myprogress prog;

    if ( curl_ )
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
            curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(curl_, CURLOPT_XFERINFOFUNCTION, progress_callback);
            curl_easy_setopt(curl_, CURLOPT_XFERINFODATA, &prog);
            curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 0L);
            curl_easy_setopt(curl_, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl_, CURLOPT_TCP_KEEPALIVE, 1L);
            curl_easy_setopt(curl_, CURLOPT_USERAGENT, "curl/7.42.0");
            curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 50L);
            curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);

            res = curl_easy_perform(curl_);
        }
        else
        {
            qDebug() << "could not write file";
        }
    }

    return 0;
}

FileDownload::~FileDownload()
{
    qDebug() << "File download complete.";
    curl_easy_cleanup(curl_);
}


