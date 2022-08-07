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

	[[nodiscard]] std::string getRequestLine() const;

	[[nodiscard]] HttpHeader getHeader() const;

	[[nodiscard]] std::string getParameter(const std::string &name) const;

	[[nodiscard]] std::unordered_map<std::string, std::string> getParameterMap() const;

public:
	URL uri;
	HttpMethod method = HttpMethod::GET;
	HttpVersion version = HttpVersion::HTTP_1_1;
	HttpHeader header;
	std::unordered_map<std::string, std::string> parameterMap;
	HttpBody *body = nullptr;
};

/********************* HttpRequestBuilder *********************/
class HttpRequestBuilder
{
public:
	class Builder
	{
	public:
		Builder() = default;

		~Builder() = default;

		Builder &url(URL &url);

		Builder &header(HttpHeader &header);

		Builder &method(HttpMethod &method);

		Builder &GET();

		Builder &POST(HttpBody *body);

		Builder &PUT(HttpBody *body);

		Builder &DELETE();

		Builder &version(HttpVersion version);

		HttpRequest build();

	private:
		HttpRequest httpRequest{};
	};

public:
	static Builder newBuilder();
};

#endif //LWHTTPD_HTTPREQUEST_H
