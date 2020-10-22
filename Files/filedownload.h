#pragma once

#include <string>

typedef void CURL;
typedef long curl_off_t;

class FileDownload
{
public:
    FileDownload();
    ~FileDownload();

    int download(std::string tag, std::string filename, std::string url, std::string authorization_token);

private:
    CURL *curl_;
};
