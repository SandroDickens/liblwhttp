#ifndef LWHTTPD_TLSCONTEXT_H
#define LWHTTPD_TLSCONTEXT_H

#include <string>

struct tls;
struct tls_config;

#define    TLS_PROTOCOL_1_0 (1 << 1) // same as TLS_PROTOCOL_TLSv1_0 in <tls.h>
#define    TLS_PROTOCOL_1_1 (1 << 2) // same as TLS_PROTOCOL_TLSv1_1 in <tls.h>
#define    TLS_PROTOCOL_1_2 (1 << 3) // same as TLS_PROTOCOL_TLSv1_2 in <tls.h>
#define    TLS_PROTOCOL_1_3 (1 << 4) // same as TLS_PROTOCOL_TLSv1_3 in <tls.h>
#define    TLS_PROTOCOL_V1 (TLS_PROTOCOL_1_0|TLS_PROTOCOL_1_1|TLS_PROTOCOL_1_2|TLS_PROTOCOL_1_3)
#define    TLS_PROTOCOL_ALL TLS_PROTOCOL_V1
#define    TLS_PROTOCOL_SAFE (TLS_PROTOCOL_1_2|TLS_PROTOCOL_1_3)

/************************ TLSContext *************************/
class TLSContextBuilder;

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

	static TLSContextBuilder *newServerBuilder();

	static TLSContextBuilder *newClientBuilder();

public:
	tls *tlsCtx = nullptr;
	tls_config *tlsConfig = nullptr;
	unsigned int protocols = TLS_PROTOCOL_1_2;
	std::vector<std::string> ciphers;
};

/********************* TLSContextBuilder *********************/
class TLSContextBuilder
{
public:
	virtual TLSContextBuilder *newServerBuilder() = 0;

	virtual TLSContextBuilder *newClientBuilder() = 0;

	virtual TLSContextBuilder *setProtocols(unsigned int protocols) = 0;

	virtual TLSContextBuilder *setCiphers(const std::vector<std::string> &ciphers) = 0;

	virtual TLSContextBuilder *setKeyFile(const std::string &keyFile) = 0;

	virtual TLSContextBuilder *setCertFile(const std::string &certFile) = 0;

	virtual TLSContext &&build() = 0;

	virtual ~TLSContextBuilder() = default;
};

class TLSContextBuilderImpl : public TLSContextBuilder
{
public:
	TLSContextBuilder *newServerBuilder() override;

	TLSContextBuilder *newClientBuilder() override;

	TLSContextBuilder *setProtocols(unsigned int protocols) override;

	TLSContextBuilder *setCiphers(const std::vector<std::string> &ciphers) override;

	TLSContextBuilder *setKeyFile(const std::string &keyFile) override;

	TLSContextBuilder *setCertFile(const std::string &certFile) override;

	TLSContext &&build() override;

private:
	TLSContext tlsContext{};
};

#endif //LWHTTPD_TLSCONTEXT_H
