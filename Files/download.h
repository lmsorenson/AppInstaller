#pragma once
#include <QObject>

#include <curl/curl.h>

typedef void CURL;
typedef long curl_off_t;

class download : public QObject
{
    Q_OBJECT

public:
    explicit download(QObject *parent = nullptr);
    download (const download &download);
    ~download();

    int run(std::string tag, std::string filename, std::string url, std::string authorization_token);

public slots:
    void on_interval();

signals:
    void make_progress(int progress);

private:
    int progress_callback(void *clientp,   curl_off_t dltotal,   curl_off_t dlnow,   curl_off_t ultotal,   curl_off_t ulnow);

    CURL *curl_;
};
