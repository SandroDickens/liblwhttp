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

	virtual size_t send(const HttpRequest &request, HttpResponse &response) = 0;

	virtual size_t sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler) = 0;

protected:
	Redirect redirect = Redirect::NORMAL;
	std::string userAgent = "lwhttp/0.0.1";
};

/*********************** HttpClientProxy *********************/
class HttpClientProxy : public HttpClient
{
public:
	~HttpClientProxy() override;

	size_t send(const HttpRequest &request, HttpResponse &response) override;

	size_t sendAsync(HttpRequest &httpRequest, std::function<HttpResponse> &responseBodyHandler) override;

private:
	HttpClient *httpClient = nullptr;
};

/******************* HttpClientNonTlsImpl ********************/
class HttpClientNonTlsImpl : public HttpClient
{
public:
	HttpClientNonTlsImpl();

	~HttpClientNonTlsImpl() override;

	size_t send(const HttpRequest &httpRequest, HttpResponse &response) override;

	size_t sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler) override;

private:
	SocketHandle socketHandle;
};

/********************* HttpClientTlsImpl *********************/
class HttpClientTlsImpl : public HttpClient
{
public:
	HttpClientTlsImpl();

	~HttpClientTlsImpl() override;

	size_t send(const HttpRequest &httpRequest, HttpResponse &response) override;

	size_t sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler) override;

private:
	SocketHandle socketHandle;
	TLSContext tlsContext;
};

/********************* HttpClientBuilder *********************/
class HttpClientBuilder
{
	class Builder
	{
	public:
		//location: https://www.google.com/xxx/
		Builder &redirect(Redirect redirect);

		Builder &userAgent(const std::string &agent);

		HttpClient *build();

	private:
		HttpClient *client = nullptr;
	};

public:
	static Builder newBuilder();
};

#endif //LWHTTPD_HTTPCLIENT_H
