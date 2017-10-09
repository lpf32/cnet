#include <cnet/net/http/HttpClient.h>
#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpResponse.h>
#include <cnet/net/EventLoop.h>

using namespace cnet;
using namespace cnet::net;

void onConnection(HttpRequest *httpRequest)
{
    httpRequest->setMethod("GET");
    httpRequest->setPath("/");
}

void onMessage(HttpResponse *httpResponse)
{
    printf("%s\n", httpResponse->getBody().c_str());
}

int main()
{
    EventLoop loop;
    HttpClient *client = HttpClient::getHttpClient(&loop, "http://www.baidu.com", "client");
    client->setHttpConnectionCallback(onConnection);
    client->setHttpMessageCallback(onMessage);
    client->connect();
    loop.loop();
}

