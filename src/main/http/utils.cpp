#include <vector>
#include "utils.h"

// Fills lps[] for given pattern pattern[0..patternLen-1]
void computeLPSArray(const char *pattern, size_t patternLen, std::vector<long> lps)
{
	// length of the previous longest prefix suffix
	long len = 0;

	lps[0] = 0; // lps[0] is always 0

	// the loop calculates lps[i] for i = 1 to patternLen-1
	size_t i = 1;
	while (i < patternLen)
	{
		if (pattern[i] == pattern[len])
		{
			len++;
			lps[i] = len;
			i++;
		}
		else // (pattern[i] != pattern[len])
		{
			// This is tricky. Consider the example. AAACAAAA and i = 7. The idea is similar to search step.
			if (len != 0)
			{
				len = lps[len - 1];
				// Also, note that we do not increment i here
			}
			else // if (len == 0)
			{
				lps[i] = 0;
				i++;
			}
		}
	}
}

std::vector<size_t> KMPSearch(const char *pattern, size_t patternLen, const char *data, size_t dataLen)
{
	std::vector<size_t> indexVec;

	// create lps[] that will hold the longest prefix suffix values for pattern
	std::vector<long> lps(patternLen);

	// Preprocess the pattern (calculate lps[] array)
	computeLPSArray(pattern, patternLen, lps);

	size_t i = 0; // index for data[]
	size_t j = 0; // index for pattern[]
	size_t index;
	while ((dataLen - i) >= (patternLen - j))
	{
		if (pattern[j] == data[i])
		{
			j++;
			i++;
		}
		if (j == patternLen)
		{
			index = i - j;
			indexVec.push_back(index);
			j = lps[j - 1];
		}
		else if (i < dataLen && pattern[j] != data[i]) // mismatch after j matches
		{
			// Do not match lps[0..lps[j-1]] characters, they will match anyway
			if (j != 0)
			{
				j = lps[j - 1];
			}
			else
			{
				i = i + 1;
			}
		}
	}
	return indexVec;
}

std::pair<bool, size_t> KMPSearchFirstOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen)
{
	std::vector<size_t> indexVec = KMPSearch(pattern, patternLen, data, dataLen);
	if (!indexVec.empty())
	{
		return {true, indexVec[0]};
	}
	else
	{
		return {false, 0};
	}
}

std::pair<bool, size_t> KMPSearchLastOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen)
{
	std::vector<size_t> indexVec = KMPSearch(pattern, patternLen, data, dataLen);
	if (!indexVec.empty())
	{
		return {true, indexVec.back()};
	}
	else
	{
		return {false, 0};
	}
}

std::vector<size_t> KMPSearchAllOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen)
{
	return KMPSearch(pattern, patternLen, data, dataLen);
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
