#ifndef LWHTTPD_HTTPREQUEST_H
#define LWHTTPD_HTTPREQUEST_H

#include <string>
#include "URL.h"
#include "HttpBase.h"


/************************ HttpRequest ************************/
class HttpRequestBuilder;

class HttpRequest
{
public:

	HttpRequest() = default;

	~HttpRequest();

	std::string getRequestLine() const;

	HttpHeader getHeader() const;

	std::string getParameter(const std::string &name) const;

	std::unordered_map<std::string, std::string> getParameterMap() const;

	static HttpRequestBuilder *newBuilder();

public:
	URL uri;
	HttpMethod method = HttpMethod::GET;
	bool expectContinue{};
	HttpVersion version = HttpVersion::HTTP_1_1;
	unsigned long timestamp{};
	HttpHeader header;
	std::unordered_map<std::string, std::string> parameterMap;
	HttpBody *body = nullptr;
};

/********************* HttpRequestBuilder *********************/
class HttpRequestBuilder
{
public:
	virtual ~HttpRequestBuilder() = default;;

	virtual HttpRequestBuilder *url(URL &url) = 0;

	virtual HttpRequestBuilder *header(HttpHeader &header) = 0;

	virtual HttpRequestBuilder *method(HttpMethod &method) = 0;

	virtual HttpRequestBuilder *GET() = 0;

	virtual HttpRequestBuilder *POST(HttpBody *body) = 0;

	virtual HttpRequestBuilder *PUT(HttpBody *body) = 0;

	virtual HttpRequestBuilder *DELETE() = 0;

	virtual HttpRequestBuilder *version(HttpVersion version) = 0;

	virtual HttpRequest build() = 0;
};

class HttpRequestBuilderImpl : public HttpRequestBuilder
{
public:
	HttpRequestBuilder *url(URL &url) override;

	HttpRequestBuilder *header(HttpHeader &header) override;

	HttpRequestBuilder *method(HttpMethod &method) override;

	HttpRequestBuilder *GET() override;

	HttpRequestBuilder *POST(HttpBody *body) override;

	HttpRequestBuilder *PUT(HttpBody *body) override;

	HttpRequestBuilder *DELETE() override;

	HttpRequestBuilder *version(HttpVersion version) override;

	HttpRequest build() override;

private:
	HttpRequest httpRequest{};
};

#endif //LWHTTPD_HTTPREQUEST_H
