#ifndef LWHTTPD_UTILS_H
#define LWHTTPD_UTILS_H

#include <cstddef>

std::pair<bool, size_t> KMPSearchFirstOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen);

std::pair<bool, size_t> KMPSearchLastOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen);

std::vector<size_t> KMPSearchAllOf(const char *pattern, size_t patternLen, const char *data, size_t dataLen);

#endif //LWHTTPD_UTILS_H
