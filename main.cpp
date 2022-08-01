#include <iostream>
#include <memory>
#include <vector>

#include "http/HttpBase.h"
#include "http/HttpRequest.h"
#include "http/TLSContext.h"
#include "http/HttpClient.h"
#include "http/HttpResponse.h"


int main()
{
	URL url("https://www.google.com/");
	std::unique_ptr<HttpRequestBuilder> requestBuilder(HttpRequest::newBuilder());
	HttpRequest request = requestBuilder->GET()->url(url)->build();

	auto var = request.getRequestLine();
	std::cout << "HTTP Request:\n" << var << std::endl;
	var = request.header.toString();
	std::cout << var << std::endl;
	/*
	std::unique_ptr<TLSContextBuilder> tlsContextBuilder(TLSContext::newClientBuilder());
	const TLSContext &context = tlsContextBuilder->setProtocols(TLS_PROTOCOL_1_2)
			->setCertFile("/etc/ssl/certs/ca-certificates.crt")
			->build();
	*/
	HttpClient *httpClient = new HttpClientProxy();
	HttpResponse response;
	httpClient->send(request, response);
	std::cout << "Status Code: " << response.getStatusCode()
	          << "\nReason: " << response.getReason()
	          << "\nContentType: " << response.getContentType()
	          << "\nContentLength: " << response.getContentLength() << std::endl;
	auto header = response.getHeader();
	std::cout << "Header:\n" << header.toString() << std::endl;
	auto contentLength = response.getContentLength();
	if (contentLength > 0)
	{
		char body[contentLength];
		memcpy(body, response.getResponseBody()->getContent(), sizeof(body));
		std::string bodyStr(body);
		std::cout << bodyStr;
	}

	return 0;
}
