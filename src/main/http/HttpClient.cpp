#include <cassert>
#include <memory>
#include <sstream>

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
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

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

/************************** Common ***************************/
constexpr long BUFFER_SIZE = 64L * 1024L;

#if defined(_WIN32) || defined(_WIN64)

void closeSocket(SocketHandle handle)
{
	shutdown(SocketHandle, SD_BOTH);
	closesocket(SocketHandle);
}
#elif defined(__linux__)

void closeSocket(SocketHandle handle)
{
	shutdown(handle, SHUT_RDWR);
	close(handle);
}

#else
#error Unsupported OS
#endif

struct GenericAddr
{
	int family;
	union UniAddr
	{
		in_addr addr4;
		in6_addr addr6;
	}addr;
};

std::vector<GenericAddr> getAddrByDomain(const std::string &hostName)
{
	std::vector<GenericAddr> addrVec;
	addrinfo hints{};
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC; // Allow IPv4 and IPv6
	hints.ai_socktype = SOCK_STREAM; // Stream socket
	hints.ai_protocol = 0;
	hints.ai_canonname = nullptr;
	hints.ai_addr = nullptr;
	hints.ai_next = nullptr;
	addrinfo *result = nullptr;
	if (0 == getaddrinfo(hostName.c_str(), nullptr, &hints, &result))
	{
		for (addrinfo *rp = result; rp != nullptr; rp = rp->ai_next)
		{
			auto addr = rp->ai_addr;
			if (addr->sa_family == AF_INET)
			{
				sockaddr_in *addr4 = reinterpret_cast<sockaddr_in *>(rp->ai_addr);
				GenericAddr genericAddr{};
				genericAddr.family = AF_INET;
				genericAddr.addr.addr4 = addr4->sin_addr;
				addrVec.push_back(genericAddr);
			}
			else if (addr->sa_family == AF_INET6)
			{
				sockaddr_in6 *addr6 = reinterpret_cast<sockaddr_in6 *>(rp->ai_addr);
				GenericAddr genericAddr{};
				genericAddr.family = AF_INET6;
				genericAddr.addr.addr6 = addr6->sin6_addr;
				addrVec.push_back(genericAddr);
			}
		}
		if (result != nullptr)
		{
			freeaddrinfo(result);
		}
	}
	return addrVec;
}

void setSocketNonBlock(SocketHandle socketHandle)
{
	int flags = fcntl(socketHandle, F_GETFL, 0);
	if (-1 != flags)
	{
		if (-1 == fcntl(socketHandle, F_SETFL, flags | O_NONBLOCK))
		{
#ifdef _DEBUG
			printf("%s,L%d,set socket flags to non-blocking failed:%d,%s\n", __func__, __LINE__, errno, strerror(errno));
#endif
		}
	}
	else
	{
#ifdef _DEBUG
		printf("%s,L%d, set socket flags failed:%d,%s\n", __func__, __LINE__, errno, strerror(errno));
#endif
	}
}

SocketHandle createIPv4Socket(const in_addr &addr, unsigned short port)
{
	SocketHandle handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (handle == INVALID_HANDLE)
	{
		return INVALID_HANDLE;
	}

	sockaddr_in remote_addr{};
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	memcpy(&remote_addr.sin_addr, &addr, sizeof(remote_addr.sin_addr));
	remote_addr.sin_port = htons(port);
	socklen_t socklen = sizeof(remote_addr);

	setSocketNonBlock(handle);

	if (-1 == connect(handle, reinterpret_cast<sockaddr *>(&remote_addr), socklen))
	{
		closeSocket(handle);
		handle = INVALID_HANDLE;
	}

	return handle;
}

SocketHandle createIPv6Socket(const in6_addr &addr, unsigned short port)
{
	SocketHandle handle = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (handle == INVALID_HANDLE)
	{
		return INVALID_HANDLE;
	}

	sockaddr_in6 remote_addr{};
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin6_family = AF_INET6;
	memcpy(&remote_addr.sin6_addr, &addr, sizeof(remote_addr.sin6_addr));
	remote_addr.sin6_port = htons(port);
	socklen_t socklen = sizeof(remote_addr);

	setSocketNonBlock(handle);

	if (-1 == connect(handle, reinterpret_cast<sockaddr *>(&remote_addr), socklen))
	{
		closeSocket(handle);
		handle = INVALID_HANDLE;
	}

	return handle;
}

SocketHandle createNonBlockSocket(const std::string &host, unsigned short port)
{
	SocketHandle socketHandle = INVALID_HANDLE;

	in_addr addr4{};
	in6_addr addr6{};

	if (1 == inet_pton(AF_INET, host.c_str(), &addr4))
	{
		socketHandle = createIPv4Socket(addr4, port);
	}
	else if (1 == inet_pton(AF_INET6, host.c_str(), &addr6))
	{
		socketHandle = createIPv6Socket(addr6, port);
	}
	else
	{
		std::vector<GenericAddr> serverAddrVec = getAddrByDomain(host);
		for (const auto tmp:serverAddrVec)
		{
			SocketHandle handle = INVALID_HANDLE;
			if (tmp.family == AF_INET)
			{
				handle = createIPv4Socket(tmp.addr.addr4, port);
			}
			else
			{
				handle = createIPv6Socket(tmp.addr.addr6, port);
			}
			if (handle != INVALID_HANDLE)
			{
				socketHandle = handle;
				break;
			}
		}
	}

	return socketHandle;
}

/******************* HttpClientNonTlsImpl ********************/
long HttpClientNonTlsImpl::send(HttpRequest &request, HttpResponse &response)
{
	if (this->socketHandle == INVALID_HANDLE)
	{
		SocketHandle handle = createNonBlockSocket(request.uri.getHost(), request.uri.getPort());
		if (handle == INVALID_HANDLE)
		{
			return -1;
		}
		this->socketHandle = handle;
	}

	std::string requestLine = request.getRequestLine();
	std::string requestHeader = request.getHeader().toString();
	std::stringstream ss;
	ss << requestLine << requestHeader;
	auto requestHeaderStr = ss.str();
	long sendLen = ::send(socketHandle, requestHeaderStr.c_str(), requestHeaderStr.length(), 0);
	if (sendLen < 0)
	{
		return -1;
	}
	if ((request.body != nullptr) && (request.body->getContentLength() > 0))
	{
		sendLen = ::send(this->socketHandle, request.body->getContent(), request.body->getContentLength(), 0);
	}
	char buffer[BUFFER_SIZE];
	long dataLen = 0;
	while (true)
	{
		long readLen = ::recv(this->socketHandle, buffer + dataLen, sizeof(buffer) - dataLen, 0);
		if (readLen > 0)
		{
			dataLen += readLen;
		}
		else
		{
			auto _errno = errno;
			if ((_errno != EAGAIN) && (_errno != EINTR))
			{
				printf("socket send error: %s(%d)\n", strerror(_errno), _errno);
				break;
			}
		}
	}
	if (dataLen > 0)
	{
		response.build(buffer, dataLen);
		return dataLen;
	}
	else
	{
		return -1;
	}
}

long HttpClientNonTlsImpl::sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler)
{
	return 0;
}

HttpClientNonTlsImpl::HttpClientNonTlsImpl()
{
	socketHandle = INVALID_HANDLE;
}

HttpClientNonTlsImpl::~HttpClientNonTlsImpl()
{
	if (socketHandle != INVALID_HANDLE)
	{
		closeSocket(socketHandle);
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
	//TODO: gethostbyname -> create socket -> connect to server -> epoll

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
	char buffer[BUFFER_SIZE];
	long dataLen = 0;
	while (true)
	{
		long readLen = tls_read(this->tlsContext.tlsCtx, buffer + dataLen, sizeof(buffer) - dataLen);
		if ((readLen != TLS_WANT_POLLIN) && (readLen != TLS_WANT_POLLOUT))
		{
			// TLS_WANT_POLLIN: The underlying read file descriptor needs to be readable in order to continue.
			// TLS_WANT_POLLOUT: The underlying write file descriptor needs to be writeable in order to continue.
			if (readLen > 0)
			{
				dataLen += readLen;
			}
			else
			{
				printf("TLS send error %ld\n", readLen);
				break;
			}
		}
	}
	if (dataLen > 0)
	{
		response.build(buffer, dataLen);
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
