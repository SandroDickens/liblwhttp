#include <cassert>
#include "HttpResponse.h"

// Fills lps[] for given pattern pattern[0..patternLen-1]
void computeLPSArray(const char *pattern, long patternLen, long *lps)
{
	// length of the previous longest prefix suffix
	long len = 0;

	lps[0] = 0; // lps[0] is always 0

	// the loop calculates lps[i] for i = 1 to patternLen-1
	long i = 1;
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

std::vector<long> KMPSearch(const char *pattern, long patternLen, const char *data, long dataLen)
{
	long index = -1;
	std::vector<long> indexVec;

	// create lps[] that will hold the longest prefix suffix values for pattern
	long lps[patternLen];

	// Preprocess the pattern (calculate lps[] array)
	computeLPSArray(pattern, patternLen, lps);

	long i = 0; // index for data[]
	long j = 0; // index for pattern[]
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

long KMPSearchFirstOf(const char *pattern, long patternLen, const char *data, long dataLen)
{
	auto index = KMPSearch(pattern, patternLen, data, dataLen);
	if (!index.empty())
	{
		return index[0];
	}
	else
	{
		return -1;
	}
}

long KMPSearchLastOf(const char *pattern, long patternLen, const char *data, long dataLen)
{
	auto index = KMPSearch(pattern, patternLen, data, dataLen);
	if (!index.empty())
	{
		return index.back();
	}
	else
	{
		return -1;
	}
}

std::vector<long> KMPSearchAllOf(const char *pattern, long patternLen, const char *data, long dataLen)
{
	return KMPSearch(pattern, patternLen, data, dataLen);
}

/************************* StatusLine ************************/
std::unordered_map<HttpStatus, std::string> StatusLine::statusCodeMap = {
		{HttpStatus::CONTINUE,                        "Continue."},
		{HttpStatus::SWITCHING_PROTOCOLS,             "Switching Protocols."},
		{HttpStatus::PROCESSING,                      "Processing."},
		{HttpStatus::CHECKPOINT,                      "Checkpoint."},
		{HttpStatus::OK,                              "OK."},
		{HttpStatus::CREATED,                         "Created."},
		{HttpStatus::ACCEPTED,                        "Accepted."},
		{HttpStatus::NON_AUTHORITATIVE_INFORMATION,   "Non-Authoritative Information."},
		{HttpStatus::NO_CONTENT,                      "No Content."},
		{HttpStatus::RESET_CONTENT,                   "Reset Content."},
		{HttpStatus::PARTIAL_CONTENT,                 "Partial Content."},
		{HttpStatus::MULTI_STATUS,                    "Multi-Status."},
		{HttpStatus::ALREADY_REPORTED,                "Already Reported."},
		{HttpStatus::IM_USED,                         "IM Used."},
		{HttpStatus::MULTIPLE_CHOICES,                "Multiple Choices."},
		{HttpStatus::MOVED_PERMANENTLY,               "Moved Permanently."},
		{HttpStatus::FOUND,                           "Found."},
		{HttpStatus::MOVED_TEMPORARILY,               "Moved Temporarily. ."},
		{HttpStatus::SEE_OTHER,                       "See Other."},
		{HttpStatus::NOT_MODIFIED,                    "Not Modified."},
		{HttpStatus::TEMPORARY_REDIRECT,              "Temporary Redirect."},
		{HttpStatus::PERMANENT_REDIRECT,              "Permanent Redirect."},
		{HttpStatus::BAD_REQUEST,                     "Bad Request."},
		{HttpStatus::UNAUTHORIZED,                    "Unauthorized."},
		{HttpStatus::PAYMENT_REQUIRED,                "Payment Required."},
		{HttpStatus::FORBIDDEN,                       "Forbidden."},
		{HttpStatus::NOT_FOUND,                       "Not Found."},
		{HttpStatus::METHOD_NOT_ALLOWED,              "Method Not Allowed."},
		{HttpStatus::NOT_ACCEPTABLE,                  "Not Acceptable."},
		{HttpStatus::PROXY_AUTHENTICATION_REQUIRED,   "Proxy Authentication Required."},
		{HttpStatus::REQUEST_TIMEOUT,                 "Request Timeout."},
		{HttpStatus::CONFLICT,                        "Conflict."},
		{HttpStatus::GONE,                            "Gone."},
		{HttpStatus::LENGTH_REQUIRED,                 "Length Required."},
		{HttpStatus::PRECONDITION_FAILED,             "Precondition failed."},
		{HttpStatus::PAYLOAD_TOO_LARGE,               "Payload Too Large."},
		{HttpStatus::REQUEST_ENTITY_TOO_LARGE,        "Request Entity Too Large."},
		{HttpStatus::REQUEST_URI_TOO_LONG,            "Request URI Too Long."},
		{HttpStatus::URI_TOO_LONG,                    "URI Too Long."},
		{HttpStatus::UNSUPPORTED_MEDIA_TYPE,          "Unsupported Media Type."},
		{HttpStatus::REQUESTED_RANGE_NOT_SATISFIABLE, "Requested Range Not Satisfiable."},
		{HttpStatus::EXPECTATION_FAILED,              "Expectation Failed."},
		{HttpStatus::I_AM_A_TEAPOT,                   "I'm a teapot."},
		{HttpStatus::UNPROCESSABLE_ENTITY,            "Unprocessable Entity."},
		{HttpStatus::LOCKED,                          "Locked."},
		{HttpStatus::FAILED_DEPENDENCY,               "Failed Dependency."},
		{HttpStatus::TOO_EARLY,                       "Too Early."},
		{HttpStatus::UPGRADE_REQUIRED,                "Upgrade Required."},
		{HttpStatus::PRECONDITION_REQUIRED,           "Precondition Required."},
		{HttpStatus::TOO_MANY_REQUESTS,               "Too Many Requests."},
		{HttpStatus::REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large."},
		{HttpStatus::UNAVAILABLE_FOR_LEGAL_REASONS,   "Unavailable For Legal Reasons."},
		{HttpStatus::INTERNAL_SERVER_ERROR,           "Internal Server Error."},
		{HttpStatus::NOT_IMPLEMENTED,                 "Not Implemented."},
		{HttpStatus::BAD_GATEWAY,                     "Bad Gateway."},
		{HttpStatus::SERVICE_UNAVAILABLE,             "Service Unavailable."},
		{HttpStatus::GATEWAY_TIMEOUT,                 "Gateway Timeout."},
		{HttpStatus::HTTP_VERSION_NOT_SUPPORTED,      "HTTP Version Not Supported."},
		{HttpStatus::VARIANT_ALSO_NEGOTIATES,         "Variant Also Negotiates."},
		{HttpStatus::INSUFFICIENT_STORAGE,            "Insufficient Storage."},
		{HttpStatus::LOOP_DETECTED,                   "Loop Detected."},
		{HttpStatus::BANDWIDTH_LIMIT_EXCEEDED,        "Bandwidth Limit Exceeded."},
		{HttpStatus::NOT_EXTENDED,                    "Not Extended."},
		{HttpStatus::NETWORK_AUTHENTICATION_REQUIRED, "Network Authentication Required."}
};

std::string StatusLine::getReason() const
{
	return statusCodeMap[status];
}

unsigned long StatusLine::build(const void *buffer, size_t len)
{
	assert(buffer != nullptr);
	assert(len > 0);
	const char statusLinePattern[] = "\r\n";
	long statusEnd = KMPSearchFirstOf(statusLinePattern, strlen(statusLinePattern), (char *)buffer, len);
	if (statusEnd <= 0)
	{
		return 0;
	}
	unsigned long statusLen = statusEnd + strlen(statusLinePattern);
	char statusLineArray[statusLen + 1];
	memcpy(statusLineArray, buffer, sizeof(statusLineArray));
	statusLineArray[statusLen] = '\0';
	std::string statusLine(statusLineArray);
	std::cout << statusLine << std::endl;
	return statusLen;
}

/************************ HttpResponse ***********************/
HttpHeader HttpResponse::getHeader() const
{
	return this->header;
}

unsigned long HttpResponse::build(const void *buffer, size_t len)
{
	assert(buffer != nullptr);
	assert(len > 0);
	//std::string line(buffer);
	const char headerPattern[] = "\r\n\r\n";
	long headerEnd = KMPSearchFirstOf(headerPattern, strlen(headerPattern), (char *)buffer, len);
	if (headerEnd <= 0)
	{
		return 0;
	}
	unsigned long prefixLen = headerEnd + strlen(headerPattern);
	char prefixArray[prefixLen + 1];
	memcpy(prefixArray, buffer, sizeof(prefixArray));
	prefixArray[prefixLen] = '\0';
	unsigned long statusLen = this->statusLine.build(prefixArray, sizeof(prefixArray));
	if (statusLen <= 0)
	{
		return 0;
	}
	unsigned long headerLen = prefixLen - statusLen;
	char headerArray[headerLen + 1];
	memcpy(headerArray, &prefixArray[statusLen], sizeof(headerArray));
	headerArray[headerLen] = '\0';
	std::string headerStr(headerArray);
	this->header.fromString(headerStr);
	const char *bodyIndex = ((char *)buffer) + prefixLen;
	body = new HttpBodyImpl(bodyIndex, this->getContentLength());
	return prefixLen + this->getContentLength();
}
