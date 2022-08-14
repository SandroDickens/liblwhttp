#include <memory>
#include <iostream>

#include <gtest/gtest.h>

#include "include/http/lwhttp.h"

void multiRequestTest()
{
	std::string urls[] = {
			"http://www.google.com",
			"http://httpbin.org/",
			"https://httpbin.org/"
	};
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent(
			"lwhttp/0.0.1").build();
	for (const auto &var: urls)
	{
		URL url = URL(var);
		HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
		HttpResponse response{};
		auto ret = client->send(request, response);
		EXPECT_GT(ret, 0);
		EXPECT_EQ(response.getStatusCode(), 200);
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
	auto ret = client->send(request, response);
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), 200);
}

void clientRequestTest_2()
{
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent(
			"lwhttp/0.0.1").build();
	URL url("https://www.youtube.com/");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	auto ret = client->send(request, response);
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), 200);
}

TEST(ClientTest, MultiClientTest)
{
	clientRequestTest_1();
	clientRequestTest_2();
}

TEST(ClientTest, MultiRequestTest)
{
	multiRequestTest();
}
