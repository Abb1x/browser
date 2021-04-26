#include "http.hpp"
extern "C"
{
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
}
#include <string>
#include <strings.h>
#include <unistd.h>
#include <utils/log.hpp>
#include <vector>

void Http::connect(std::string url, int port)
{
    const char *name_c = url.c_str();

    struct hostent host = *gethostbyname(name_c);

    struct sockaddr_in address;

    address.sin_family = AF_INET;

    address.sin_port = htons(port); // Port 80 (web)

    bcopy((char *)host.h_addr, (char *)&address.sin_addr.s_addr, host.h_length);

    Http::socket_ = socket(AF_INET, SOCK_STREAM, 0);

    if (Http::socket_ == -1)
    {
        log(ERROR, "Cannot create the boob..");
    }

    if (::connect(Http::socket_, (const struct sockaddr *)&address, sizeof(address)) != 0)
    {
        log(ERROR, "Connection failure, please boobulate again later");
    }

    log(INFO, "Connected to %s:%d via HTTP", name_c, port);
}

std::string Http::make_request(std::string request)
{
    constexpr auto BUFFER_SIZE = 10000;

    send(Http::socket_, request.c_str(), std::string(request).length(), 0);

    usleep(2000);

    std::vector<char> buffer(BUFFER_SIZE);

    read(Http::socket_, buffer.data(), buffer.size());

    return buffer.data();
}

std::string Http::drop_headers(std::string document)
{
    int location = document.find("\r\n\r\n");

    std::string pure = document.substr(location + 4);

    return pure;
}

std::string Http::entry_point(std::string url, int port)
{
    connect(url, port);

    std::string request = "GET /index.html HTTP/1.1\nHost: " + url + "\n\n";

    std::string response = make_request(request);

    response = drop_headers(response);

    return response;
}
