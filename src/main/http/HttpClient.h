#ifndef LWHTTPD_HTTPCLIENT_H
#define LWHTTPD_HTTPCLIENT_H

#include <functional>

#include "TLSContext.h"

class HttpRequest;

class HttpResponse;

/************************ Common *************************/
#if defined(_WIN32) || defined(_WIN64)
#include <WinSock2.h>
using SocketHandle = SOCKET;
#define INVALID_HANDLE INVALID_SOCKET
#elif defined(__linux__)
using SocketHandle = int;
#define INVALID_HANDLE (-1)
#else
#error Unsupported OS
#endif

/************************ HttpClient *************************/
class HttpClient
{
public:
	virtual ~HttpClient() = default;

	friend class HttpClientBuilder;

	virtual long send(HttpRequest &request, HttpResponse &response) = 0;

	virtual long sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler) = 0;
};

/*********************** HttpClientProxy *********************/
class HttpClientProxy : public HttpClient
{
public:
	~HttpClientProxy() override;

	long send(HttpRequest &request, HttpResponse &response) override;

	long sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler) override;

private:
	HttpClient *httpClient = nullptr;
};

/******************* HttpClientNonTlsImpl ********************/
class HttpClientNonTlsImpl : public HttpClient
{
public:
	HttpClientNonTlsImpl();

	~HttpClientNonTlsImpl() override;

	long send(HttpRequest &request, HttpResponse &response) override;

	long sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler) override;

private:
	SocketHandle socketHandle;
};

/********************* HttpClientTlsImpl *********************/
class HttpClientTlsImpl : public HttpClient
{
public:
	HttpClientTlsImpl();

	~HttpClientTlsImpl() override = default;

	long send(HttpRequest &request, HttpResponse &response) override;

	long sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler) override;

private:
	TLSContext tlsContext;
};

/********************* HttpClientBuilder *********************/
class HttpClientBuilder
{
public:
};

#endif //LWHTTPD_HTTPCLIENT_H
