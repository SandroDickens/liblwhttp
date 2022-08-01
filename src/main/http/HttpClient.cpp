#include <cassert>

#include <tls.h>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpClient.h"

#if defined(_WIN32) || defined(_WIN64)
#include <WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#endif

#if defined(__linux__)

#include <unistd.h>
#include <sys/socket.h>
#include <memory>
#include <sstream>

#endif


/************************ HttpClient *************************/

/*********************** HttpClientProxy *********************/
HttpClientProxy::~HttpClientProxy()
{
	if (this->httpClient != nullptr)
	{
		delete this->httpClient;
	}
}

long HttpClientProxy::send(HttpRequest &request, HttpResponse &response)
{
	if (request.uri.getScheme() == Scheme::Https)
	{
		this->httpClient = new HttpClientTlsImpl();
	}
	else
	{
		this->httpClient = new HttpClientNonTlsImpl();
	}
	return this->httpClient->send(request, response);
}

long HttpClientProxy::sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler)
{
	return 0;
}

constexpr long BUFFER_SIZE = 4L * 1024L * 1024L;

/******************* HttpClientNonTlsImpl ********************/
long HttpClientNonTlsImpl::send(HttpRequest &request, HttpResponse &response)
{
	return 0;
}

long HttpClientNonTlsImpl::sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler)
{
	return 0;
}

HttpClientNonTlsImpl::HttpClientNonTlsImpl()
{
	sessionHandle = INVALID_HANDLE;
}

HttpClientNonTlsImpl::~HttpClientNonTlsImpl()
{
	if (sessionHandle != INVALID_HANDLE)
	{
#if defined(_WIN32) || defined(_WIN64)
		shutdown(sessionHandle, SD_BOTH);
		closesocket(sessionHandle);
#endif
#if defined(__linux__)
		shutdown(sessionHandle, SHUT_RDWR);
		close(sessionHandle);
#endif
	}
}

/********************* HttpClientTlsImpl *********************/
long HttpClientTlsImpl::send(HttpRequest &request, HttpResponse &response)
{
	assert((this->tlsContext.tlsCtx != nullptr) && (this->tlsContext.tlsConfig != nullptr));
	std::string requestLine = request.getRequestLine();
	std::string requestHeader = request.getHeader().toString();
	std::string host = request.uri.getHost();
	unsigned short port = request.uri.getPort();
	std::string _port = std::to_string(port);
	if (-1 == tls_connect(this->tlsContext.tlsCtx, host.c_str(), _port.c_str()))
	{
		return -1;
	}
	std::stringstream ss;
	ss << requestLine << requestHeader;
	auto requestHeaderStr = ss.str();
	long sendLen = tls_write(this->tlsContext.tlsCtx, requestHeaderStr.c_str(), requestHeaderStr.length());
	if (sendLen < 0)
	{
		tls_close(this->tlsContext.tlsCtx);
		return -1;
	}
	if ((request.body != nullptr) && (request.body->getContentLength() > 0))
	{
		sendLen = tls_write(this->tlsContext.tlsCtx, request.body->getContent(), request.body->getContentLength());
	}
	std::array<char, BUFFER_SIZE> buffer{};
	while (true)
	{
		long readLen = tls_read(this->tlsContext.tlsCtx, buffer.data(), buffer.size());
		if ((readLen != TLS_WANT_POLLIN) && (readLen != TLS_WANT_POLLOUT))
		{
			// TLS_WANT_POLLIN: The underlying read file descriptor needs to be readable in order to continue.
			// TLS_WANT_POLLOUT: The underlying write file descriptor needs to be writeable in order to continue.
			if (readLen > 0)
			{
				response.build(buffer.data(), readLen);
				break;
			}
			else
			{
				printf("TLS send error %ld\n", readLen);
				break;
			}
		}
	}
	return 0;
}

long HttpClientTlsImpl::sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler)
{
	return 0;
}

HttpClientTlsImpl::HttpClientTlsImpl()
{
	std::unique_ptr<TLSContextBuilder> tlsContextBuilder(TLSContext::newClientBuilder());
	this->tlsContext = tlsContextBuilder->setProtocols(TLS_PROTOCOL_SAFE)->build();
}
