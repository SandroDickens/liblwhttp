#ifndef LWHTTP_TLSCONTEXT_H
#define LWHTTP_TLSCONTEXT_H

#include <string>
#include <vector>

typedef struct ssl_st SSL;
typedef struct ssl_ctx_st SSL_CTX;

struct ssl_ctx_st;
struct ssl_st;

enum class TLSProtocol
{
	TLSv1_1 = 0x01,
	TLSv1_2 = 0x02,
	TLSv1_3 = 0x04
};

constexpr int TLSv1 = (int)TLSProtocol::TLSv1_1|(int)TLSProtocol::TLSv1_2|(int)TLSProtocol::TLSv1_3;

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

	[[nodiscard]] TLSProtocol getProtocols() const;

	[[nodiscard]] std::vector<std::string> getCiphers() const;

private:
	struct Initializer
	{
		Initializer();

		~Initializer();
	};

public:
	SSL_CTX *sslCtx = nullptr;
	SSL *ssl = nullptr;
	TLSProtocol tlsProtocol = TLSProtocol::TLSv1_2;
	std::vector<std::string> ciphers;
	static Initializer initializer;
};

/********************* TLSContextBuilder *********************/
class TLSContextBuilder
{
public:
	class Builder
	{
	public:
		Builder &newClientBuilder();

		Builder &setMinVersion(TLSProtocol protocol);

		TLSContext build();

	private:
		TLSContext tlsContext{};
	};

public:
	static Builder newBuilder();
};

#endif //LWHTTP_TLSCONTEXT_H
