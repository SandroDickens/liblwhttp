#include <cassert>
#include <sstream>
#include "HttpResponse.h"
#include "utils.h"

/************************* StatusLine ************************/
std::unordered_map<HttpStatus, std::string> StatusLine::statusCodeMap = {
		{HttpStatus::CONTINUE,                        "Continue"},
		{HttpStatus::SWITCHING_PROTOCOLS,             "Switching Protocols"},
		{HttpStatus::PROCESSING,                      "Processing"},
		{HttpStatus::CHECKPOINT,                      "Checkpoint"},
		{HttpStatus::OK,                              "OK"},
		{HttpStatus::CREATED,                         "Created"},
		{HttpStatus::ACCEPTED,                        "Accepted"},
		{HttpStatus::NON_AUTHORITATIVE_INFORMATION,   "Non-Authoritative Information"},
		{HttpStatus::NO_CONTENT,                      "No Content"},
		{HttpStatus::RESET_CONTENT,                   "Reset Content"},
		{HttpStatus::PARTIAL_CONTENT,                 "Partial Content"},
		{HttpStatus::MULTI_STATUS,                    "Multi-Status"},
		{HttpStatus::ALREADY_REPORTED,                "Already Reported"},
		{HttpStatus::IM_USED,                         "IM Used"},
		{HttpStatus::MULTIPLE_CHOICES,                "Multiple Choices"},
		{HttpStatus::MOVED_PERMANENTLY,               "Moved Permanently"},
		{HttpStatus::FOUND,                           "Found"},
		{HttpStatus::MOVED_TEMPORARILY,               "Moved Temporarily. "},
		{HttpStatus::SEE_OTHER,                       "See Other"},
		{HttpStatus::NOT_MODIFIED,                    "Not Modified"},
		{HttpStatus::TEMPORARY_REDIRECT,              "Temporary Redirect"},
		{HttpStatus::PERMANENT_REDIRECT,              "Permanent Redirect"},
		{HttpStatus::BAD_REQUEST,                     "Bad Request"},
		{HttpStatus::UNAUTHORIZED,                    "Unauthorized"},
		{HttpStatus::PAYMENT_REQUIRED,                "Payment Required"},
		{HttpStatus::FORBIDDEN,                       "Forbidden"},
		{HttpStatus::NOT_FOUND,                       "Not Found"},
		{HttpStatus::METHOD_NOT_ALLOWED,              "Method Not Allowed"},
		{HttpStatus::NOT_ACCEPTABLE,                  "Not Acceptable"},
		{HttpStatus::PROXY_AUTHENTICATION_REQUIRED,   "Proxy Authentication Required"},
		{HttpStatus::REQUEST_TIMEOUT,                 "Request Timeout"},
		{HttpStatus::CONFLICT,                        "Conflict"},
		{HttpStatus::GONE,                            "Gone"},
		{HttpStatus::LENGTH_REQUIRED,                 "Length Required"},
		{HttpStatus::PRECONDITION_FAILED,             "Precondition failed"},
		{HttpStatus::PAYLOAD_TOO_LARGE,               "Payload Too Large"},
		{HttpStatus::REQUEST_ENTITY_TOO_LARGE,        "Request Entity Too Large"},
		{HttpStatus::REQUEST_URI_TOO_LONG,            "Request URI Too Long"},
		{HttpStatus::URI_TOO_LONG,                    "URI Too Long"},
		{HttpStatus::UNSUPPORTED_MEDIA_TYPE,          "Unsupported Media Type"},
		{HttpStatus::REQUESTED_RANGE_NOT_SATISFIABLE, "Requested Range Not Satisfiable"},
		{HttpStatus::EXPECTATION_FAILED,              "Expectation Failed"},
		{HttpStatus::I_AM_A_TEAPOT,                   "I'm a teapot"},
		{HttpStatus::UNPROCESSABLE_ENTITY,            "Unprocessable Entity"},
		{HttpStatus::LOCKED,                          "Locked"},
		{HttpStatus::FAILED_DEPENDENCY,               "Failed Dependency"},
		{HttpStatus::TOO_EARLY,                       "Too Early"},
		{HttpStatus::UPGRADE_REQUIRED,                "Upgrade Required"},
		{HttpStatus::PRECONDITION_REQUIRED,           "Precondition Required"},
		{HttpStatus::TOO_MANY_REQUESTS,               "Too Many Requests"},
		{HttpStatus::REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large"},
		{HttpStatus::UNAVAILABLE_FOR_LEGAL_REASONS,   "Unavailable For Legal Reasons"},
		{HttpStatus::INTERNAL_SERVER_ERROR,           "Internal Server Error"},
		{HttpStatus::NOT_IMPLEMENTED,                 "Not Implemented"},
		{HttpStatus::BAD_GATEWAY,                     "Bad Gateway"},
		{HttpStatus::SERVICE_UNAVAILABLE,             "Service Unavailable"},
		{HttpStatus::GATEWAY_TIMEOUT,                 "Gateway Timeout"},
		{HttpStatus::HTTP_VERSION_NOT_SUPPORTED,      "HTTP Version Not Supported"},
		{HttpStatus::VARIANT_ALSO_NEGOTIATES,         "Variant Also Negotiates"},
		{HttpStatus::INSUFFICIENT_STORAGE,            "Insufficient Storage"},
		{HttpStatus::LOOP_DETECTED,                   "Loop Detected"},
		{HttpStatus::BANDWIDTH_LIMIT_EXCEEDED,        "Bandwidth Limit Exceeded"},
		{HttpStatus::NOT_EXTENDED,                    "Not Extended"},
		{HttpStatus::NETWORK_AUTHENTICATION_REQUIRED, "Network Authentication Required"}
};

std::string StatusLine::getReason() const
{
	return statusCodeMap[status];
}

size_t StatusLine::build(const char *buffer, size_t len)
{
	assert(buffer != nullptr);
	assert(len > 0);
	size_t result = 0;
	const char statusLinePattern[] = "\r\n";
	std::pair<bool, size_t> resultPair = KMPSearchFirstOf(statusLinePattern, strlen(statusLinePattern), buffer, len);
	if (resultPair.first)
	{
		size_t statusLen = resultPair.second + strlen(statusLinePattern);
		//char statusLineArray[statusLen + 1];
		std::vector<char> statusLineVec(statusLen + 1);
		memcpy(statusLineVec.data(), buffer, statusLineVec.size());
		statusLineVec[statusLen] = '\0';
		std::string statusLine(statusLineVec.data());
		try
		{
			fromString(statusLine);
			result = statusLen;
		}
		catch (std::exception &e)
		{
			throw e;
		}
	}
	return result;
}

void StatusLine::fromString(const std::string &str)
{
	size_t begin = 0, end;
	end = str.find_first_of(' ');
	if (end == std::string::npos)
	{
		throw std::invalid_argument("Bad HTTP status line format");
	}
	std::string versionStr = str.substr(begin, end - begin);
	if (versionStr == "HTTP/1.1")
	{
		version = HttpVersion::HTTP_1_1;
	}
	else if (versionStr == "HTTP/2")
	{
		version = HttpVersion::HTTP2;
	}
	else if (versionStr == "HTTP/1.0")
	{
		version = HttpVersion::HTTP_1_0;
	}
	else
	{
		throw std::invalid_argument("Invalid HTTP version " + versionStr);
	}

	begin = str.find_first_not_of(' ', end);
	if (end == std::string::npos)
	{
		throw std::invalid_argument("Bad HTTP status line format");
	}
	end = str.find_first_of(' ', begin);
	if (end == std::string::npos)
	{
		throw std::invalid_argument("Bad HTTP status line format");
	}
	std::string code = str.substr(begin, end - begin);
	try
	{
		status = static_cast<HttpStatus>(std::stoi(code));
	}
	catch (std::exception &e)
	{
		throw std::invalid_argument("Invalid HTTP status code " + code);
	}

	begin = str.find_first_not_of(' ', end);
	if (end == std::string::npos)
	{
		throw std::invalid_argument("Bad HTTP status line format");
	}
	end = str.find_first_of("\r\n", begin);
	if (end == std::string::npos)
	{
		throw std::invalid_argument("Bad HTTP status line format");
	}
	std::string reason = str.substr(begin, end - begin);
#ifdef _DEBUG
	if (reason != statusCodeMap.at(status))
	{
		std::cout << "Reason-Phrase '" << reason << "/" << statusCodeMap.at(status) << "' maybe incorrect" << std::endl;
	}
#endif
}

/************************ HttpResponse ***********************/
HttpResponse::~HttpResponse()
{
	if (body != nullptr)
	{
		delete body;
	}
}

HttpHeader HttpResponse::getHeader() const
{
	return this->header;
}

static size_t getHttpHeader(const char *buffer, unsigned long len)
{
	size_t headLen = 0;
	const char headerPattern[] = "\r\n\r\n";
	std::pair<bool, size_t> resultPair = KMPSearchFirstOf(headerPattern, strlen(headerPattern), buffer, len);
	if (resultPair.first)
	{
		headLen = resultPair.second + strlen(headerPattern);
	}
	return headLen;
}

size_t HttpResponse::buildHeader(const char *buffer, size_t len)
{
	size_t headLen = getHttpHeader(buffer, len);
	if (headLen > 0)
	{
		try
		{
			size_t statusLen = this->statusLine.build(buffer, headLen);
			const std::string headStr(buffer + statusLen);
			this->header.fromString(headStr);
			return headLen;
		}
		catch (std::exception &e)
		{
			throw e;
		}
	}
	return 0;
}

void HttpResponse::build(const char *buffer, size_t bodyLen)
{
	assert(buffer != nullptr);
	assert(bodyLen > 0);
//	auto value = header.getField("Content-Length");
//	if (!value.empty())
//	{
//		size_t contentLen = stoul(value);
//		assert(contentLen == bodyLen);
//	}
	this->body = new HttpBodyImpl(buffer, bodyLen);
	this->body->setBodyLength(bodyLen);
}
