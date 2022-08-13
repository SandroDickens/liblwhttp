#include <sstream>
#include <utility>
#include "../../include/http/HttpBase.h"
#include "../../include/http/HttpRequest.h"

/************************ HttpRequest ************************/
HttpRequest::~HttpRequest()
{
	parameterMap.clear();
}

std::string HttpRequest::getRequestLine() const
{
	std::stringstream ss;
	switch (method)
	{
		case HttpMethod::GET:
			ss << "GET";
			break;
		case HttpMethod::POST:
			ss << "POST";
			break;
		case HttpMethod::PUT:
			ss << "PUT";
			break;
		case HttpMethod::DELETE:
			ss << "DELETE";
			break;
	}
	ss << " " << uri.getPath();
	std::string query = uri.getQuery();
	if (!query.empty())
	{
		ss << "?" << query;
	}
	ss << " " << HttpVersionSerialize(version) << "\r\n";
	return ss.str();
}

HttpHeader HttpRequest::getHeader() const
{
	return header;
}

std::string HttpRequest::getParameter(const std::string &name) const
{
	if (parameterMap.cend() != parameterMap.find(name))
	{
		return parameterMap.at(name);
	}
	else
	{
		return {};
	}
}

std::unordered_map<std::string, std::string> HttpRequest::getParameterMap() const
{
	return parameterMap;
}

/********************* HttpRequestBuilder *********************/
HttpRequestBuilder::Builder HttpRequestBuilder::newBuilder()
{
	return HttpRequestBuilder::Builder{};
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::url(URL &url)
{
	std::string host = url.getHost();
	if (url.getPort() != 0)
	{
		host.append(":");
		host.append(std::to_string(url.getPort()));
	}
	this->httpRequest.header.setField("Host", host);
	this->httpRequest.uri = url;
	return *this;
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::header(HttpHeader &header)
{
	this->httpRequest.header = header;
	return *this;
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::method(HttpMethod &method)
{
	this->httpRequest.method = method;
	return *this;
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::GET()
{
	this->httpRequest.method = HttpMethod::GET;
	return *this;
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::POST(std::shared_ptr<HttpBody> body)
{
	this->httpRequest.method = HttpMethod::POST;
	this->httpRequest.body = std::move(body);
	return *this;
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::PUT(std::shared_ptr<HttpBody> body)
{
	this->httpRequest.method = HttpMethod::PUT;
	this->httpRequest.body = std::move(body);
	return *this;
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::DELETE()
{
	this->httpRequest.method = HttpMethod::DELETE;
	return *this;
}

HttpRequestBuilder::Builder &HttpRequestBuilder::Builder::version(HttpVersion version)
{
	this->httpRequest.version = version;
	return *this;
}

HttpRequest HttpRequestBuilder::Builder::build()
{
	this->httpRequest.header.setField("User-Agent", "lwhttp/0.0.1");
	this->httpRequest.header.setField("Accept", "*/*");
	return this->httpRequest;
}
