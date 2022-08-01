#ifndef LWHTTPD_URI_H
#define LWHTTPD_URI_H

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
	URL() = default;

	explicit URL(const std::string &str);

	[[nodiscard]] std::string toString() const;

	void initialize();

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
	Scheme scheme = Scheme::Null;
	std::string authInfo;
	std::string host;
	unsigned short port{};
	std::string path;
	std::string query;
};

#endif //LWHTTPD_URI_H
