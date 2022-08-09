#ifndef LWHTTP_UTILS_H
#define LWHTTP_UTILS_H

#include <cstddef>
#include <string>

std::pair<bool, size_t> KMPSearchFirstOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen);

std::pair<bool, size_t> KMPSearchLastOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen);

std::vector<size_t> KMPSearchAllOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen);

void ltrim(std::string &s);

void ltrim(std::string &s, char ch);

void rtrim(std::string &s);

void rtrim(std::string &s, char ch);

void trim(std::string &s);

void trim(std::string &s, char ch);

#endif //LWHTTP_UTILS_H
