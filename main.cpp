#include <iostream>
#include <memory>
#include <vector>

#include "http/HttpBase.h"
#include "http/HttpRequest.h"
#include "http/HttpClient.h"
#include "http/HttpResponse.h"


int main()
{
	std::shared_ptr<HttpClient> httpClient = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent(
			"lwhttp/0.0.1").build();

	URL url("https://www.google.com/");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();

	auto var = request.getRequestLine();
	std::cout << "HTTP Request: " << var << std::endl;
	var = request.header.serialize();
	std::cout << var << std::endl;

	HttpResponse response{};
	httpClient->send(request, response);
	std::cout << "Status Code: " << response.getStatusCode()
	          << "\nReason: " << response.getReason()
	          << "\nContentType: " << response.getContentType()
	          << "\nBody Length: " << response.getBodyLength() << std::endl;
	auto header = response.getHeader();
	std::cout << "Header:\n" << header.serialize() << std::endl;
	auto bodyLength = response.getBodyLength();
	if (bodyLength > 0)
	{
		std::vector<char> body(bodyLength + 1);
		memcpy(body.data(), response.getResponseBody()->getContent(), bodyLength);
		body[bodyLength] = '\0';
		std::string bodyStr(body.data());
		//std::cout << bodyStr;
	}

	url = URL("https://www.qq.com/");
	request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response2{};
	httpClient->send(request, response2);
	std::cout << "Status Code: " << response2.getStatusCode()
	          << "\nReason: " << response2.getReason()
	          << "\nContentType: " << response2.getContentType()
	          << "\nBody Length: " << response2.getBodyLength() << std::endl;
	header = response2.getHeader();
	std::cout << "Header:\n" << header.serialize() << std::endl;
	bodyLength = response2.getBodyLength();
	if (bodyLength > 0)
	{
		std::vector<char> body(bodyLength + 1);
		memcpy(body.data(), response2.getResponseBody()->getContent(), bodyLength);
		body[bodyLength] = '\0';
		std::string bodyStr(body.data());
		//std::cout << bodyStr;
	}

	url = URL("http://10.0.0.6:8080");
	request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	var = request.getRequestLine();
	std::cout << "HTTP Request: " << var << std::endl;
	var = request.header.serialize();
	std::cout << var << std::endl;

	HttpResponse response3{};
	httpClient->send(request, response3);
	std::cout << "Status Code: " << response3.getStatusCode()
	          << "\nReason: " << response3.getReason()
	          << "\nContentType: " << response3.getContentType()
	          << "\nBody Length: " << response3.getBodyLength() << std::endl;
	header = response3.getHeader();
	std::cout << "Header:\n" << header.serialize() << std::endl;
	bodyLength = response3.getBodyLength();
	if (bodyLength > 0)
	{
		std::vector<char> body(bodyLength + 1);
		memcpy(body.data(), response3.getResponseBody()->getContent(), bodyLength);
		body[bodyLength] = '\0';
		std::string bodyStr(body.data());
		//std::cout << bodyStr;
	}

	HttpResponse response4{};
	httpClient->send(request, response4);
	std::cout << "Status Code: " << response4.getStatusCode()
	          << "\nReason: " << response4.getReason()
	          << "\nContentType: " << response4.getContentType()
	          << "\nBody Length: " << response4.getBodyLength() << std::endl;
	header = response4.getHeader();
	std::cout << "Header:\n" << header.serialize() << std::endl;
	bodyLength = response4.getBodyLength();
	if (bodyLength > 0)
	{
		std::vector<char> body(bodyLength + 1);
		memcpy(body.data(), response4.getResponseBody()->getContent(), bodyLength);
		body[bodyLength] = '\0';
		std::string bodyStr(body.data());
		std::cout << bodyStr;
	}
	return 0;
}
