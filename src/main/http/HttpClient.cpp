#include <cassert>
#include <memory>
#include <sstream>

#include <openssl/ssl.h>

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpClient.h"
#include "utils.h"

#if defined(_WIN32) || defined(_WIN64)

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#endif

#if defined(__linux__)

#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#endif

/************************** Common ***************************/
struct VariableArray
{
	VariableArray()
	{
		length = BUFFER_SIZE;
		buffer = new char[length];
		memset(buffer, 0, length);
	}

	~VariableArray()
	{
		delete[] buffer;
	}

	void expand()
	{
		size_t newLength = length << 2;
		char *newBuff = new char[newLength];
		memset(newBuff, 0, newLength);
		memcpy(newBuff, buffer, length);
		buffer = newBuff;
		length = newLength;
	}

	static constexpr long BUFFER_SIZE = 64L * 1024L;
	size_t length;
	char *buffer;
};

#if defined(_WIN32) || defined(_WIN64)

void closeSocket(SocketHandle handle)
{
	shutdown(handle, SD_BOTH);
	closesocket(handle);
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
	} addr;
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
#ifdef _WIN32
	unsigned long non_block = 1;
	if (NO_ERROR != ioctlsocket(socketHandle, FIONBIO, &non_block))
	{
		int dwErrNo = WSAGetLastError();
		printf("%s,L%d,set socket flags to non-blocking failed! error:%d\n", __func__, __LINE__, dwErrNo);
	}
#elif __linux__
	int flags = fcntl(socketHandle, F_GETFL, 0);
	if (-1 != flags)
	{
		if (-1 == fcntl(socketHandle, F_SETFL, flags | O_NONBLOCK))
		{
#ifdef _DEBUG
			printf("%s,L%d,set socket flags to non-blocking failed: %s(%d)\n", __func__, __LINE__, strerror(errno),
				   errno);
#endif
		}
	}
	else
	{
#ifdef _DEBUG
		printf("%s,L%d, set socket flags failed: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
	}
#endif
}

SocketHandle createIPv4Socket(const in_addr &addr, unsigned short port, bool async)
{
	SocketHandle handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (handle == INVALID_FD)
	{
#ifdef _DEBUG
#ifdef _WIN32
		printf("%s, L%d, socket create error: %d\n", __func__, __LINE__, WSAGetLastError());
#elif __linux__
		printf("%s, L%d, socket create error: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
#endif
		return INVALID_FD;
	}

	sockaddr_in remote_addr{};
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	memcpy(&remote_addr.sin_addr, &addr, sizeof(remote_addr.sin_addr));
	remote_addr.sin_port = htons(port);
	socklen_t socklen = sizeof(remote_addr);

	if (-1 == connect(handle, reinterpret_cast<sockaddr *>(&remote_addr), socklen))
	{
#ifdef _DEBUG
#ifdef _WIN32
		printf("%s, L%d, server error: %d\n", __func__, __LINE__, WSAGetLastError());
#elif __linux__
		printf("%s, L%d, connect to server error: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
#endif
		closeSocket(handle);
		handle = INVALID_FD;
	}

	if (async)
	{
		setSocketNonBlock(handle);
	}

	return handle;
}

SocketHandle createIPv6Socket(const in6_addr &addr, unsigned short port, bool async)
{
	SocketHandle handle = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (handle == INVALID_FD)
	{
#ifdef _DEBUG
#ifdef _WIN32
		printf("%s, L%d, socket create error: %d\n", __func__, __LINE__, WSAGetLastError());
#elif __linux__
		printf("%s, L%d, socket create error: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
#endif
		return INVALID_FD;
	}

	sockaddr_in6 remote_addr{};
	memset(&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin6_family = AF_INET6;
	memcpy(&remote_addr.sin6_addr, &addr, sizeof(remote_addr.sin6_addr));
	remote_addr.sin6_port = htons(port);
	socklen_t socklen = sizeof(remote_addr);

	if (-1 == connect(handle, reinterpret_cast<sockaddr *>(&remote_addr), socklen))
	{
#ifdef _DEBUG
#ifdef _WIN32
		printf("%s, L%d, server error: %d\n", __func__, __LINE__, WSAGetLastError());
#elif __linux__
		printf("%s, L%d, connect to server error: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
#endif
		closeSocket(handle);
		handle = INVALID_FD;
	}

	if (async)
	{
		setSocketNonBlock(handle);
	}
	return handle;
}

SocketHandle createSocket(const std::string &host, unsigned short port, bool async)
{
	SocketHandle socketHandle = INVALID_FD;

	in_addr addr4{};
	in6_addr addr6{};

	if (1 == inet_pton(AF_INET, host.c_str(), &addr4))
	{
		socketHandle = createIPv4Socket(addr4, port, async);
	}
	else if (1 == inet_pton(AF_INET6, host.c_str(), &addr6))
	{
		socketHandle = createIPv6Socket(addr6, port, async);
	}
	else
	{
		std::vector<GenericAddr> serverAddrVec = getAddrByDomain(host);
		for (const auto &tmp: serverAddrVec)
		{
			SocketHandle handle = INVALID_FD;
			if (tmp.family == AF_INET)
			{
				handle = createIPv4Socket(tmp.addr.addr4, port, async);
			}
			else
			{
				handle = createIPv6Socket(tmp.addr.addr6, port, async);
			}
			if (handle != INVALID_FD)
			{
				socketHandle = handle;
				break;
			}
		}
	}

	return socketHandle;
}

/************************ HttpClient *************************/
HttpClient::HttpClient()
{
	redirect = Redirect::NORMAL;
	userAgent = "lwhttp/0.0.1";
	timeout = DEFAULT_TIMEOUT;
}

/*********************** HttpClientProxy *********************/
std::mutex HttpClientProxy::clientMutex;
std::shared_ptr<HttpClient> HttpClientProxy::httpClient = nullptr;
std::shared_ptr<HttpClient> HttpClientProxy::httpsClient = nullptr;

size_t HttpClientProxy::send(const HttpRequest &request, HttpResponse &response)
{
	HttpClient *client;
	if (request.uri.getScheme() == Scheme::Https)
	{
		if (HttpClientProxy::httpsClient == nullptr)
		{
			clientMutex.lock();
			if (HttpClientProxy::httpsClient == nullptr)
			{
				HttpClientProxy::httpsClient = std::make_shared<HttpClientTlsImpl>();
			}
			clientMutex.unlock();
		}
		client = HttpClientProxy::httpsClient.get();
	}
	else
	{
		if (HttpClientProxy::httpClient == nullptr)
		{
			clientMutex.lock();
			if (HttpClientProxy::httpClient == nullptr)
			{
				HttpClientProxy::httpClient = std::make_shared<HttpClientNonTlsImpl>();
			}
			clientMutex.unlock();
		}
		client = HttpClientProxy::httpClient.get();
	}

	return client->send(request, response);
}

size_t HttpClientProxy::sendAsync(HttpRequest &httpRequest, std::function<HttpResponse> &responseBodyHandler)
{
	return 0;
}

/******************* HttpClientNonTlsImpl ********************/
size_t HttpClientNonTlsImpl::send(const HttpRequest &httpRequest, HttpResponse &response)
{
	SocketHandle socketHandle = createSocket(httpRequest.uri.getHost(), httpRequest.uri.getPort(), false);
	if (socketHandle == INVALID_FD)
	{
		return 0;
	}

	std::string requestLine = httpRequest.getRequestLine();
	HttpHeader header = httpRequest.getHeader();
	header.setField("user-agent", this->userAgent);
	std::string requestStr = requestLine + header.serialize();
#ifdef _WIN32
	int len = static_cast<int>(requestStr.length());
#else
	size_t len =requestStr.length();
#endif
	long sendLen = ::send(socketHandle, requestStr.c_str(), len, 0);
	if (sendLen < 0)
	{
#ifdef _DEBUG
#ifdef _WIN32
		printf("%s:%d send request header failed:: %d\n", __func__, __LINE__, WSAGetLastError());
#elif __linux__
		printf("%s:%d send request header failed:: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
#endif
		return 0;
	}
	if ((httpRequest.body != nullptr) && (httpRequest.body->getBodyLength() > 0))
	{
#ifdef _WIN32
		len = static_cast<int>(httpRequest.body->getBodyLength());
#else
		len = httpRequest.body->getBodyLength()
#endif
		sendLen = ::send(socketHandle, httpRequest.body->getContent(), len, 0);
		if (sendLen < 0)
		{
#ifdef _DEBUG
#ifdef _WIN32
			printf("%s:%d send request body failed:: %d\n", __func__, __LINE__, WSAGetLastError());
#elif __linux__
			printf("%s:%d send request body failed:: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
#endif
		}
	}

	size_t headLen = 0;
	size_t dataLen = 0;
	bool isChunked = false;
	size_t contentLen = 0;
	VariableArray array;
	while (true)
	{
#ifdef _WIN32
		len = static_cast<int>(array.length - dataLen);
#else
		len = array.length - dataLen;
#endif
		long readLen = ::recv(socketHandle, array.buffer + dataLen, len, 0);
		if (readLen <= 0)
		{
#ifdef _WIN32
			auto erroCode = WSAGetLastError();
			if ((erroCode != WSAEWOULDBLOCK) && (erroCode != WSAEINTR))
			{
#ifdef _DEBUG
				printf("%s:%d, socket send error: %d\n", __func__, __LINE__, erroCode);
#endif
				break;
			}
#elif __linux__
			auto _t_errno = errno;
			if ((_t_errno != 0) && (_t_errno != EINTR))
			{
#ifdef _DEBUG
				printf("%s:%d, socket send error: %s(%d)\n", __func__, __LINE__, strerror(errno), errno);
#endif
				break;
			}
#endif
		}
		else
		{
			dataLen += readLen;
			if (dataLen >= array.length)
			{
				array.expand();
			}
			if (headLen == 0)
			{
				headLen = response.buildHeader(array.buffer, dataLen);
				char tmpBuff[VariableArray::BUFFER_SIZE];
				memcpy(tmpBuff, array.buffer + headLen, dataLen - headLen);
				dataLen = dataLen - headLen;
				memset(array.buffer, 0, array.length);
				memcpy(array.buffer, tmpBuff, dataLen);
				auto tmp = response.getHeader().getField("Content-Length");
				if (!tmp.empty())
				{
					contentLen = std::stoul(tmp);
				}
				if (contentLen == 0)
				{
					std::string transEnc = response.getHeader().getField("Transfer-Encoding");
					if (std::string::npos != transEnc.find("chunked"))
					{
						isChunked = true;
					}
				}
			}
			if (contentLen != 0)
			{
				if (dataLen >= contentLen)
				{
					break;
				}
			}
			else if (isChunked)
			{
				char contentEnd[] = "0\r\n\r\n";
				auto resultPair = KMPSearchFirstOf(contentEnd, strlen(contentEnd), array.buffer, dataLen);
				if (resultPair.first)
				{
					dataLen = resultPair.second;
					break;
				}
			}
		}
	}

	closeSocket(socketHandle);
	if (dataLen > 0)
	{
		response.build(array.buffer, dataLen);
		return dataLen;
	}
	else
	{
		return 0;
	}
}

size_t HttpClientNonTlsImpl::sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler)
{
	return 0;
}

HttpClientNonTlsImpl::HttpClientNonTlsImpl()
{
#ifdef _WIN32
	WSAData stWSAData{};
	if (0 != WSAStartup(MAKEWORD(2, 2), &stWSAData))
	{
		auto wsaError = WSAGetLastError();
		printf("WSAStartup failed! %d\n", wsaError);
	}
#endif
}

HttpClientNonTlsImpl::~HttpClientNonTlsImpl()
{
#ifdef _WIN32
	WSACleanup();
#endif
}


/********************* HttpClientTlsImpl *********************/
size_t HttpClientTlsImpl::send(const HttpRequest &httpRequest, HttpResponse &response)
{
	assert(this->tlsContext.ssl != nullptr);
	SSL *dupSSL = SSL_dup(this->tlsContext.ssl);
	std::string host = httpRequest.uri.getHost();

	SocketHandle socketHandle = createSocket(httpRequest.uri.getHost(), httpRequest.uri.getPort(), false);
	if (socketHandle == INVALID_FD)
	{
		return 0;
	}

	SSL_set_fd(dupSSL, (int)socketHandle);
	int var = SSL_connect(dupSSL);
	if (var != 1)
	{
#ifdef _DEBUG
		int ssl_errno = SSL_get_error(dupSSL, var);
		printf("%s:%d tls connect to server failed: %d\n", __func__, __LINE__, ssl_errno);
#endif
		SSL_free(dupSSL);
		closeSocket(socketHandle);
		return 0;
	}
	std::string requestLine = httpRequest.getRequestLine();
	HttpHeader header = httpRequest.getHeader();
	header.setField("user-agent", this->userAgent);
	std::string requestStr = requestLine + header.serialize();
	size_t sendLen = 0;
	var = SSL_write_ex(dupSSL, requestStr.c_str(), requestStr.length(), &sendLen);
	if (0 == var)
	{
#ifdef _DEBUG
		int ssl_errno = SSL_get_error(dupSSL, var);
		printf("%s:%d send request header failed: %d\n", __func__, __LINE__, ssl_errno);
#endif
		SSL_shutdown(dupSSL);
		SSL_free(dupSSL);
		closeSocket(socketHandle);
		return 0;
	}
	if ((httpRequest.body != nullptr) && (httpRequest.body->getBodyLength() > 0))
	{
		size_t written = 0;
		var = SSL_write_ex(dupSSL, httpRequest.body->getContent(), httpRequest.body->getBodyLength(), &written);
		if (0 == var)
		{
#ifdef _DEBUG
			int ssl_errno = SSL_get_error(dupSSL, var);
			printf("%s:%d send request body failed: %d\n", __func__, __LINE__, ssl_errno);
#endif
			SSL_shutdown(dupSSL);
			SSL_free(dupSSL);
			closeSocket(socketHandle);
			return 0;
		}
		sendLen += written;
	}

	size_t headLen = 0;
	size_t dataLen = 0;
	bool isChunked = false;
	size_t contentLen = 0;
	VariableArray array;
	while (true)
	{
		size_t readBytes = 0;
		var = SSL_read_ex(dupSSL, array.buffer + dataLen, array.length - dataLen, &readBytes);
		if (0 == var)
		{
#ifdef _DEBUG
			int ssl_errno = SSL_get_error(dupSSL, var);
			printf("%s:%d read response failed: %d\n", __func__, __LINE__, ssl_errno);
#endif
			break;
		}
		else
		{
			dataLen += readBytes;
			if (dataLen >= array.length)
			{
				array.expand();
			}
			if (headLen == 0)
			{
				headLen = response.buildHeader(array.buffer, dataLen);
				char tmpBuff[VariableArray::BUFFER_SIZE];
				memcpy(tmpBuff, array.buffer + headLen, dataLen - headLen);
				dataLen = dataLen - headLen;
				memset(array.buffer, 0, array.length);
				memcpy(array.buffer, tmpBuff, dataLen);
				auto tmp = response.getHeader().getField("Content-Length");
				if (!tmp.empty())
				{
					contentLen = std::stoul(tmp);
				}
				if (contentLen == 0)
				{
					std::string transEnc = response.getHeader().getField("Transfer-Encoding");
					if (std::string::npos != transEnc.find("chunked"))
					{
						isChunked = true;
					}
				}
			}
			if (contentLen != 0)
			{
				if (dataLen >= contentLen)
				{
					break;
				}
			}
			else if (isChunked)
			{
				char contentEnd[] = "0\r\n\r\n";
				auto resultPair = KMPSearchFirstOf(contentEnd, strlen(contentEnd), array.buffer, dataLen);
				if (resultPair.first)
				{
					dataLen = resultPair.second;
					break;
				}
			}
		}
	}
	SSL_shutdown(dupSSL);
	SSL_free(dupSSL);
	closeSocket(socketHandle);
	if (dataLen > 0)
	{
		response.build(array.buffer, dataLen);
		return dataLen;
	}
	else
	{
		return 0;
	}
}

size_t HttpClientTlsImpl::sendAsync(HttpRequest &request, std::function<HttpResponse> &responseBodyHandler)
{
	return 0;
}

HttpClientTlsImpl::HttpClientTlsImpl()
{
#ifdef _WIN32
	WSAData stWSAData{};
	if (0 != WSAStartup(MAKEWORD(2, 2), &stWSAData))
	{
		auto wsaError = WSAGetLastError();
		printf("WSAStartup failed! %d\n", wsaError);
	}
#endif
	this->tlsContext = TLSContextBuilder::newBuilder().newClientBuilder().setMinVersion(TLSv1_1).build();
}

HttpClientTlsImpl::~HttpClientTlsImpl()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

/********************* HttpClientBuilder *********************/
HttpClientBuilder::Builder HttpClientBuilder::newBuilder()
{
	HttpClientBuilder::Builder builder{};
	return builder;
}

HttpClientBuilder::Builder &HttpClientBuilder::Builder::redirect(Redirect redirect)
{
	if (this->client == nullptr)
	{
		this->client = std::make_shared<HttpClientProxy>();
	}
	this->client->redirect = redirect;
	return *this;
}

HttpClientBuilder::Builder &HttpClientBuilder::Builder::userAgent(const std::string &agent)
{
	if (this->client == nullptr)
	{
		this->client = std::make_shared<HttpClientProxy>();
	}
	this->client->userAgent = agent;
	return *this;
}

HttpClientBuilder::Builder &HttpClientBuilder::Builder::timeout(unsigned int seconds)
{
	if (seconds > 0)
	{
		this->client->timeout = seconds;
	}
	return *this;
}

std::shared_ptr<HttpClient> HttpClientBuilder::Builder::build()
{
	return this->client;
}
