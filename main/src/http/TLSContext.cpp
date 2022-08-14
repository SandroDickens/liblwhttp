#include <cassert>
#include <stdexcept>
#include <vector>

#include <openssl/ssl.h>

#include "../../include/http/TLSContext.h"


long tlsProtocolToVersion(TLSProtocol protocol)
{
	long version;
	switch (protocol)
	{
		case TLSProtocol::TLSv1_1:
			version = TLS1_1_VERSION;
			break;
		case TLSProtocol::TLSv1_2:
			version = TLS1_2_VERSION;
			break;
		case TLSProtocol::TLSv1_3:
			version = TLS1_3_VERSION;
			break;
	}
	return version;
}

/************************ TLSContext *************************/
TLSContext::Initializer::Initializer()
{
	OPENSSL_init_ssl(OPENSSL_INIT_LOAD_SSL_STRINGS, nullptr);
}

TLSContext::Initializer::~Initializer()
{
	CRYPTO_cleanup_all_ex_data();
}

TLSContext::TLSContext(TLSContext &&other) noexcept
{
	if (this != &other)
	{
		this->sslCtx = other.sslCtx;
		other.sslCtx = nullptr;
		this->ssl = other.ssl;
		other.ssl = nullptr;
		this->tlsProtocol = other.tlsProtocol;
		this->ciphers = std::move(other.ciphers);
	}
}

TLSContext &TLSContext::operator=(TLSContext &&other) noexcept
{
	if (this != &other)
	{
		this->sslCtx = other.sslCtx;
		other.sslCtx = nullptr;
		this->ssl = other.ssl;
		other.ssl = nullptr;
		this->tlsProtocol = other.tlsProtocol;
		this->ciphers = std::move(other.ciphers);
	}
	return *this;
}

TLSContext::~TLSContext()
{
	if (ssl != nullptr)
	{
		SSL_shutdown(ssl);
		SSL_free(ssl);
	}
	if (sslCtx != nullptr)
	{
		SSL_CTX_free(sslCtx);
	}
}

TLSProtocol TLSContext::getProtocols() const
{
	return tlsProtocol;
}

std::vector<std::string> TLSContext::getCiphers() const
{
	return this->ciphers;
}

/********************* TLSContextBuilder *********************/
TLSContextBuilder::Builder &TLSContextBuilder::Builder::newClientBuilder()
{
	const SSL_METHOD *sslMethod = TLS_client_method();
	SSL_CTX *sslCtx = SSL_CTX_new(sslMethod);
	if (sslCtx == nullptr)
	{
		throw std::runtime_error("SSL context create failed!");
	}
	this->tlsContext.sslCtx = sslCtx;

	tlsContext.ssl = SSL_new(sslCtx);
	if (tlsContext.ssl == nullptr)
	{
		throw std::runtime_error("SSL create failed!");
	}
	if (0 == SSL_set_min_proto_version(this->tlsContext.ssl, tlsProtocolToVersion(this->tlsContext.tlsProtocol)))
	{
		std::string exceptWhat("Set TLS version error!");
		throw std::runtime_error(exceptWhat);
	}
	return *this;
}

TLSContextBuilder::Builder &TLSContextBuilder::Builder::setMinVersion(TLSProtocol protocol)
{
	assert(this->tlsContext.ssl != nullptr);

	if (0 == SSL_set_min_proto_version(this->tlsContext.ssl, tlsProtocolToVersion(protocol)))
	{
		std::string exceptWhat("Set TLS version error!");
		throw std::runtime_error(exceptWhat);
	}
	this->tlsContext.tlsProtocol = protocol;
	return *this;
}

TLSContext TLSContextBuilder::Builder::build()
{
	assert(this->tlsContext.ssl != nullptr);
	return std::move(this->tlsContext);
}

TLSContextBuilder::Builder TLSContextBuilder::newBuilder()
{
	return TLSContextBuilder::Builder{};
}
