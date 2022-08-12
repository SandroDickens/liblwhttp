#include "http/URL.h"
#include "http/HttpClient.h"
#include "http/HttpRequest.h"
#include "http/HttpResponse.h"

void multiRequestTest()
{
	std::string urls[] = {
			"http://www.google.com",
			"http://httpbin.org/",
			"https://httpbin.org/"
	};
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent(
			"lwhttp/0.0.1").build();
	for (const auto &var:urls)
	{
		URL url = URL(var);
		HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
		HttpResponse response{};
		client->send(request, response);
		std::cout << "status: " << response.getStatusCode() << "/" << response.getReason() << std::endl;
		//std::cout << response.getResponseBody()->getContent();
	}
}

void clientRequestTest_1()
{
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent(
			"lwhttp/0.0.1").build();
	URL url("https://www.google.com");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	client->send(request, response);
}

void clientRequestTest_2()
{
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent(
			"lwhttp/0.0.1").build();
	URL url("https://www.youtube.com/");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	client->send(request, response);
}

void multiClientTest()
{
	clientRequestTest_1();
	clientRequestTest_2();
}

int ClientTests()
{
	multiRequestTest();
	multiClientTest();

	return 0;
}