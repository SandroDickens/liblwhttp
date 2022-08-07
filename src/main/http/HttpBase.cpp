#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>

#include "HttpBase.h"

void toUpCamelCase(std::string &str)
{
	str[0] = static_cast<char>(std::toupper(static_cast<int>(str[0])));
	bool isFirstLetter = false;
	for (auto &ch: str)
	{
		if (isFirstLetter)
		{
			ch = (char)std::toupper(ch);
		}
		if (ch == ' ' || ch == '-')
		{
			isFirstLetter = true;
		}
		else
		{
			isFirstLetter = false;
		}
	}
}

void toUpCase(std::string &str)
{
	for (auto &ch: str)
	{
		ch = (char)std::toupper(ch);
	}
}

void toLowCase(std::string &str)
{
	for (auto &ch: str)
	{
		ch = (char)std::tolower(ch);
	}
}

/************************* HttpHeader ************************/
std::string HttpHeader::toString() const
{
	std::string str;
	for (const auto &field: fieldsMap)
	{
		std::string name = field.first;
		toUpCamelCase(name);
		str += name + ": " + field.second + "\r\n";
	}
	return str + "\r\n";
}

void HttpHeader::fromString(const std::string &str)
{
	std::string _str = str;
	_str.erase(std::remove(_str.begin(), _str.end(), '\r'), _str.end());
	size_t begin = 0;
	size_t end;
	do
	{
		end = _str.find_first_of('\n', begin);
		if (begin >= end)
		{
			break;
		}
		std::string field = _str.substr(begin, end - begin);
		begin = end + 1;
		std::string name, value;
		auto delimiter = field.find_first_of(':');
		if (delimiter == std::string::npos)
		{
			throw std::invalid_argument(R"(The format of the field should be "name: value\r\n")");
		}
		name = field.substr(0, delimiter);
		std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c)
		{ return std::tolower(c); });
		value = field.substr(delimiter + 1);
		value.erase(std::remove_if(value.begin(), value.end(), isspace), value.end());
		fieldsMap.insert({name, value});
	}
	while (std::string::npos != end);
}

std::string HttpHeader::getField(const std::string &name) const
{
	std::string _name = name;
	std::transform(_name.begin(), _name.end(), _name.begin(), [](unsigned char c)
	{ return std::tolower(c); });
	if (fieldsMap.cend() != fieldsMap.find(_name))
	{
		return fieldsMap.at(_name);
	}
	else
	{
		return {};
	}
}

void HttpHeader::setField(const std::string &name, const std::string &value)
{
	std::string _name = name;
	std::transform(_name.begin(), _name.end(), _name.begin(), [](unsigned char c)
	{ return std::tolower(c); });
	fieldsMap.insert_or_assign(_name, value);
}

/************************** Cookie ***************************/
void Cookie::setCookie(const std::string &name, std::string &value)
{
	cookiesMap.insert_or_assign(name, value);
}

std::string Cookie::getCookie(const std::string &name)
{
	if (cookiesMap.cend() != cookiesMap.find(name))
	{
		return cookiesMap.at(name);
	}
	else
	{
		return {};
	}
}

std::vector<std::pair<std::string, std::string>> Cookie::getCookies()
{
	std::vector<std::pair<std::string, std::string>> cookiesVec;
	for (auto &cookie: cookiesMap)
	{
		cookiesVec.emplace_back(cookie.first, cookie.second);
	}
	return cookiesVec;
}

bool Cookie::hasCookie(const std::string &name)
{
	return cookiesMap.cend() != cookiesMap.find(name);
}
