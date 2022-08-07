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
	//URL url("http://10.0.0.2/wiki/");
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
	/*
	std::unique_ptr<HttpClient> httpClient(new HttpClientProxy());
	 */
	HttpClient *httpClient = HttpClientBuilder::newBuilder().redirect(
			Redirect::NORMAL).userAgent("lwhttp/0.0.1").build();
	HttpResponse response;
	httpClient->send(request, response);
	std::cout << "Status Code: " << response.getStatusCode()
	          << "\nReason: " << response.getReason()
	          << "\nContentType: " << response.getContentType()
	          << "\nBody Length: " << response.getBodyLength() << std::endl;
	auto header = response.getHeader();
	std::cout << "Header:\n" << header.toString() << std::endl;
	auto bodyLength = response.getBodyLength();
	if (bodyLength > 0)
	{
		std::vector<char> body(bodyLength + 1);
		memcpy(body.data(), response.getResponseBody()->getContent(), body.size());
		body[bodyLength] = '\0';
		std::string bodyStr(body.data());
		std::cout << bodyStr;
	}

	return 0;
}
