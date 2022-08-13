#ifndef LWHTTP_URI_H
#define LWHTTP_URI_H

#include <string>

enum class Scheme
{
	Null,
	Http,
	Https
};

class URL
{
public:
	URL();

	explicit URL(const std::string &str);

	[[nodiscard]] std::string serialize() const;

	[[nodiscard]] Scheme getScheme() const
	{
		return scheme;
	}

	[[nodiscard]] std::string getAuthInfo() const
	{
		return authInfo;
	}

	[[nodiscard]] std::string getHost() const
	{
		return host;
	}

	[[nodiscard]] unsigned short getPort() const
	{
		return port;
	}

	[[nodiscard]] std::string getPath() const
	{
		return path;
	}

	[[nodiscard]] std::string getQuery() const
	{
		return query;
	}

private:
	void initialize();

private:
	Scheme scheme = Scheme::Null;
	std::string authInfo;
	std::string host;
	unsigned short port{};
	std::string path;
	std::string query;
};

#endif //LWHTTP_URI_H
