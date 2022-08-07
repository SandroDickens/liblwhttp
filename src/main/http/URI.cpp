#include <regex>
#include <iostream>
#include <sstream>

#include "URL.h"

URL::URL(const std::string &str)
{
	this->initialize();

	std::regex regexAuth(R"((https?):\/\/([^:\s]+):([^@\s]+)@([^\/:\s]+)(:\d+)?(\/[^?\s]*)?(\?\S+)?)");
	std::regex regexNoAuth(R"((https?):\/\/([^\/:\s]+)(:\d+)?(\/[^?\s]*)?(\?\S+)?)");
	std::smatch matchResults;
	std::string schemeStr, hostStr, portStr, pathStr, queryStr;
	if (std::regex_match(str, matchResults, regexAuth))
	{
		unsigned int SCHEME_GRP = 1, USERNAME_GRP = 2, PASSWD_GRP = 3, HOST_GRP = 4, PORT_GRP = 5, PATH_GRP = 6, QUERY_GRP = 7;
		schemeStr = matchResults[SCHEME_GRP];
		if ((matchResults.length(USERNAME_GRP) != 0) && (matchResults.length(PASSWD_GRP) != 0))
		{
			authInfo = matchResults[USERNAME_GRP].str() + "@" + matchResults[PASSWD_GRP].str();
		}
		hostStr = matchResults[HOST_GRP];
		portStr = matchResults[PORT_GRP];
		pathStr = matchResults[PATH_GRP];
		queryStr = matchResults[QUERY_GRP];
	}
	else if (std::regex_match(str, matchResults, regexNoAuth))
	{
		unsigned int SCHEME_GRP = 1, HOST_GRP = 2, PORT_GRP = 3, PATH_GRP = 4, QUERY_GRP = 5;
		schemeStr = matchResults[SCHEME_GRP];
		hostStr = matchResults[HOST_GRP];
		portStr = matchResults[PORT_GRP];
		pathStr = matchResults[PATH_GRP];
		queryStr = matchResults[QUERY_GRP];
	}
	else
	{
		throw std::invalid_argument("Unrecognized URL.");
	}
	if (schemeStr == "http")
	{
		scheme = Scheme::Http;
	}
	else if (schemeStr == "https")
	{
		scheme = Scheme::Https;
	}
	else
	{
		throw std::invalid_argument("Only supports http and https.");
	}
	if (!hostStr.empty())
	{
		host = hostStr;
	}
	else
	{
		throw std::invalid_argument("Host is essential.");
	}
	if (!portStr.empty())
	{
		port = std::stoi(portStr.substr(1));
	}
	else
	{
		if (scheme == Scheme::Http)
		{
			port = 80;
		}
		else
		{
			port = 443;
		}
	}
	if (!pathStr.empty())
	{
		path = pathStr;
	}
	else
	{
		path = "/";
	}
	if (!queryStr.empty())
	{
		query = queryStr.substr(1);
	}
}

std::string URL::toString() const
{
	std::stringstream ss;
	switch (scheme)
	{
		case Scheme::Http:
			ss << "http://";
			break;
		case Scheme::Https:
			ss << "https://";
			break;
		case Scheme::Null:
		default:
			break;
	}
	if (!authInfo.empty())
	{
		size_t delimiter = authInfo.find_first_of('@');
		ss << authInfo.substr(0, delimiter) << ":" << authInfo.substr(delimiter + 1) + "@";
	}
	ss << host;
	if (((scheme == Scheme::Http) && (port != 80)) ||
	    ((scheme == Scheme::Https) && (port != 443)))
	{
		ss << ":" << port;
	}
	ss << path;
	if (!query.empty())
	{
		ss << "?" << query;
	}
	return ss.str();
}

void URL::initialize()
{
	scheme = Scheme::Null;
	authInfo = {};
	host = {};
	port = 0;
	path = {};
	query = {};
}
