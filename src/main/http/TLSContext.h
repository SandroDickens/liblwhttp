#ifndef LWHTTP_TLSCONTEXT_H
#define LWHTTP_TLSCONTEXT_H

#include <string>
#include <openssl/ssl.h>

#define TLSv1   TLS1_VERSION
#define TLSv1_1 TLS1_1_VERSION
#define TLSv1_2 TLS1_2_VERSION
#define TLSv1_3 TLS1_3_VERSION

/************************ TLSContext *************************/
class TLSContext
{
public:
	TLSContext() = default;

	TLSContext(const TLSContext &other) = delete;

	TLSContext &operator=(const TLSContext &other) = delete;

	TLSContext(TLSContext &&other) noexcept;

	TLSContext &operator=(TLSContext &&other) noexcept;

	virtual ~TLSContext();

	[[nodiscard]] unsigned int getProtocols() const;

	[[nodiscard]] std::vector<std::string> getCiphers() const;

public:
	SSL_CTX *sslCtx = nullptr;
	SSL *ssl = nullptr;
	unsigned int version = TLSv1;
	std::vector<std::string> ciphers;
};

/********************* TLSContextBuilder *********************/
class TLSContextBuilder
{
public:
	class Builder
	{
	public:
		Builder &newClientBuilder();

		Builder &setMinVersion(unsigned int version);

		TLSContext build();

	private:
		TLSContext tlsContext{};
	};

public:
	static Builder newBuilder();
};

#endif //LWHTTP_TLSCONTEXT_H
