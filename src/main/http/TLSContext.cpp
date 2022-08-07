#include <cassert>
#include <stdexcept>
#include <tls.h>
#include <vector>

#include "TLSContext.h"

/************************ TLSContext *************************/
TLSContext::TLSContext(TLSContext &&other) noexcept
{
	if (this != &other)
	{
		this->tlsCtx = other.tlsCtx;
		other.tlsCtx = nullptr;
		this->tlsConfig = other.tlsConfig;
		other.tlsConfig = nullptr;
		this->protocols = other.protocols;
		other.protocols = 0;
		this->ciphers = std::move(other.ciphers);
	}
}

TLSContext &TLSContext::operator=(TLSContext &&other) noexcept
{
	if (this != &other)
	{
		this->tlsCtx = other.tlsCtx;
		other.tlsCtx = nullptr;
		this->tlsConfig = other.tlsConfig;
		other.tlsConfig = nullptr;
		this->protocols = other.protocols;
		other.protocols = 0;
		this->ciphers = std::move(other.ciphers);
	}
	return *this;
}

TLSContext::~TLSContext()
{
	if (tlsCtx != nullptr)
	{
		tls_close(tlsCtx);
		tls_free(tlsCtx);
		tlsCtx = nullptr;
	}
	if (tlsConfig != nullptr)
	{
		tls_config_free(tlsConfig);
		tlsConfig = nullptr;
	}
}

unsigned int TLSContext::getProtocols() const
{
	return protocols;
}

std::vector<std::string> TLSContext::getCiphers() const
{
	return this->ciphers;
}

TLSContextBuilder *TLSContext::newServerBuilder()
{
	return (new TLSContextBuilderImpl())->newServerBuilder();
}

TLSContextBuilder *TLSContext::newClientBuilder()
{
	return (new TLSContextBuilderImpl())->newClientBuilder();
}

/********************* TLSContextBuilder *********************/
TLSContextBuilder *TLSContextBuilderImpl::newServerBuilder()
{
	if (-1 == tls_init())
	{
		throw std::runtime_error("TLS initial failed!");
	}
	tlsContext.tlsCtx = tls_server();
	if (tlsContext.tlsCtx == nullptr)
	{
		throw std::runtime_error("TLS context create failed!");
	}
	tlsContext.tlsConfig = tls_config_new();
	if (tlsContext.tlsConfig == nullptr)
	{
		tls_free(tlsContext.tlsCtx);
		tlsContext.tlsCtx = nullptr;
		throw std::runtime_error("TLS config create failed!");
	}
	return this;
}

TLSContextBuilder *TLSContextBuilderImpl::newClientBuilder()
{
	if (-1 == tls_init())
	{
		throw std::runtime_error("TLS initial failed!");
	}
	tlsContext.tlsCtx = tls_client();
	if (tlsContext.tlsCtx == nullptr)
	{
		throw std::runtime_error("TLS context create failed!");
	}
	tlsContext.tlsConfig = tls_config_new();
	if (tlsContext.tlsConfig == nullptr)
	{
		tls_free(tlsContext.tlsCtx);
		tlsContext.tlsCtx = nullptr;
		throw std::runtime_error("TLS config create failed!");
	}
	return this;
}

TLSContextBuilder *TLSContextBuilderImpl::setProtocols(unsigned int protocols)
{
	assert(this->tlsContext.tlsConfig != nullptr);
	if (-1 == tls_config_set_protocols(tlsContext.tlsConfig, protocols))
	{
		std::string exceptWhat("TLS set protocols error!");
		throw std::runtime_error(exceptWhat + tls_config_error(this->tlsContext.tlsConfig));
	}
	this->tlsContext.protocols = protocols;
	return this;
}

TLSContextBuilder *TLSContextBuilderImpl::setCiphers(const std::vector<std::string> &ciphers)
{
	assert(this->tlsContext.tlsConfig != nullptr);
	std::string _ciphers;
	for (const std::string &cipher: ciphers)
	{
		_ciphers.append(":" + cipher);
	}
	_ciphers.erase(_ciphers.begin());
	if (-1 == tls_config_set_ciphers(tlsContext.tlsConfig, _ciphers.c_str()))
	{
		std::string exceptWhat("TLS set protocols error!");
		throw std::runtime_error(exceptWhat + tls_config_error(this->tlsContext.tlsConfig));
	}
	this->tlsContext.ciphers = ciphers;
	return this;
}

TLSContextBuilder *TLSContextBuilderImpl::setKeyFile(const std::string &keyFile)
{
	assert(this->tlsContext.tlsConfig != nullptr);
	if (-1 == tls_config_set_key_file(tlsContext.tlsConfig, keyFile.c_str()))
	{
		std::string exceptWhat("TLS set key file error!");
		throw std::runtime_error(exceptWhat + tls_config_error(this->tlsContext.tlsConfig));
	}
	return this;
}

TLSContextBuilder *TLSContextBuilderImpl::setCertFile(const std::string &certFile)
{
	assert(this->tlsContext.tlsConfig != nullptr);
	if (-1 == tls_config_set_cert_file(tlsContext.tlsConfig, certFile.c_str()))
	{
		std::string exceptWhat("TLS set key file error!");
		throw std::runtime_error(exceptWhat + tls_config_error(this->tlsContext.tlsConfig));
	}
	return this;
}

TLSContext &&TLSContextBuilderImpl::build()
{
	assert(this->tlsContext.tlsConfig != nullptr);
	return std::move(this->tlsContext);
}
