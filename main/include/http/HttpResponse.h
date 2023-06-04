#ifndef LWHTTP_HTTPRESPONSE_H
#define LWHTTP_HTTPRESPONSE_H

#include <iostream>
#include <unordered_map>

#include "HttpBase.h"

/************************* HttpStatus ************************/
enum class HttpStatus
{
	CONTINUE = 100,
	SWITCHING_PROTOCOLS = 101,
	PROCESSING = 102,
	CHECKPOINT = 103,
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTI_STATUS = 207,
	ALREADY_REPORTED = 208,
	IM_USED = 226,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	MOVED_TEMPORARILY = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT = 308,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTHENTICATION_REQUIRED = 407,
	REQUEST_TIMEOUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PRECONDITION_FAILED = 412,
	PAYLOAD_TOO_LARGE = 413,
	REQUEST_ENTITY_TOO_LARGE = 413,
	REQUEST_URI_TOO_LONG = 414,
	URI_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	REQUESTED_RANGE_NOT_SATISFIABLE = 416,
	EXPECTATION_FAILED = 417,
	I_AM_A_TEAPOT = 418,
	UNPROCESSABLE_ENTITY = 422,
	LOCKED = 423,
	FAILED_DEPENDENCY = 424,
	TOO_EARLY = 425,
	UPGRADE_REQUIRED = 426,
	PRECONDITION_REQUIRED = 428,
	TOO_MANY_REQUESTS = 429,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	UNAVAILABLE_FOR_LEGAL_REASONS = 451,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	VARIANT_ALSO_NEGOTIATES = 506,
	INSUFFICIENT_STORAGE = 507,
	LOOP_DETECTED = 508,
	BANDWIDTH_LIMIT_EXCEEDED = 509,
	NOT_EXTENDED = 510,
	NETWORK_AUTHENTICATION_REQUIRED = 511
};

std::string HttpStatusSerialize(HttpStatus status);

HttpStatus HttpStatusDeserialize(const std::string &str);

std::ostream &operator<<(std::ostream &os, const HttpStatus &obj);

/************************* StatusLine ************************/
class StatusLine : public Serializable
{
public:
	[[nodiscard]] std::string serialize() const override;

	[[nodiscard]] HttpVersion getVersion() const
	{ return version; }

	[[nodiscard]] HttpStatus getStatusCode() const
	{
		return status;
	}

	[[nodiscard]] std::string getReason() const;

	size_t build(const char *buffer, size_t len);

private:
	void deserialize(const std::string &str) override;

private:
	HttpVersion version{};
	HttpStatus status{};
};

/************************ HttpResponse ***********************/
class HttpResponse
{
public:
	~HttpResponse();

	[[nodiscard]] StatusLine getStatusLine() const
	{
		return statusLine;
	}

	[[nodiscard]] HttpStatus getStatusCode() const
	{
		return statusLine.getStatusCode();
	}

	[[nodiscard]] HttpVersion getVersion() const
	{
		return statusLine.getVersion();
	}

	[[nodiscard]] std::string getReason() const
	{
		return statusLine.getReason();
	}

	[[nodiscard]] HttpHeader getHeader() const;

	[[nodiscard]] std::string getContentType() const
	{
		return header.getField("Content-Type");
	}

	[[nodiscard]] size_t getBodyLength() const
	{
		if (body == nullptr)
		{
			return 0;
		}
		else
		{
			return body->getBodyLength();
		}
	}

	[[nodiscard]] HttpBody *getResponseBody() const
	{
		return body;
	}

	size_t buildHeader(const char *buffer, size_t len);

	void build(const char *buffer, size_t bodyLen);

private:
	StatusLine statusLine{};
	HttpHeader header{};
	HttpBody *body = nullptr;
};

#endif //LWHTTP_HTTPRESPONSE_H
