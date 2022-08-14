#include <algorithm>
#include <cstring>
#include <string>


std::pair<bool, size_t> findFirstOf(const char *target, size_t targetLen, const char *data, size_t dataLen)
{
	if (targetLen > dataLen)
	{
		return {false, 0};
	}

	std::pair<bool, size_t> found{false, 0};
	for (size_t index = 0; index <= dataLen - targetLen; ++index)
	{
		if (target[0] == data[index])
		{
			if (0 == memcmp(target, data + index, targetLen))
			{
				found.first = true;
				found.second = index;
				break;
			}
		}
	}
	return found;
}

std::pair<bool, size_t> findLastOf(const char *target, size_t targetLen, const char *data, size_t dataLen)
{
	if (targetLen > dataLen)
	{
		return {false, 0};
	}

	std::pair<bool, size_t> found{false, 0};
	for (size_t index = 0; index <= dataLen - targetLen; ++index)
	{
		if (target[0] == data[index])
		{
			if (0 == memcmp(target, data + index, targetLen))
			{
				found.first = true;
				found.second = index;
			}
		}
	}
	return found;
}

void ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
	{ return !std::isspace(ch); }));
}

void ltrim(std::string &s, char ch)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [ch](unsigned char c)
	{ return ch != c; }));
}

void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
	{ return !std::isspace(ch); }).base(), s.end());
}

void rtrim(std::string &s, char ch)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [ch](unsigned char c)
	{ return ch != c; }).base(), s.end());
}

void trim(std::string &s)
{
	ltrim(s);
	rtrim(s);
}

void trim(std::string &s, char ch)
{
	ltrim(s, ch);
	rtrim(s, ch);
}
