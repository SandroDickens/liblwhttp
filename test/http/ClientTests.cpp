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
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().userAgent("lwhttp/0.0.1").build();
	for (const auto &var: urls)
	{
		URL url = URL(var);
		HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
		HttpResponse response{};
		auto ret = client->send(request, response);
		EXPECT_GT(ret, 0);
		EXPECT_EQ(response.getStatusCode(), HttpStatus::OK);
		//std::cout << response.getResponseBody()->getContent();
	}
}

void clientRequestTest_1()
{
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().userAgent("lwhttp/0.0.1").build();
	URL url("https://www.google.com");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	auto ret = client->send(request, response);
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), HttpStatus::OK);
}

void clientRequestTest_2()
{
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().userAgent("lwhttp/0.0.1").build();
	URL url("https://www.youtube.com/");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	auto ret = client->send(request, response);
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), HttpStatus::OK);
}

TEST(ClientTest, MultiClientTest)
{
	clientRequestTest_1();
	clientRequestTest_2();
}

TEST(RequestTests, GetMethod)
{
	URL url("http://10.0.0.6/demo.php?id=1");
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().userAgent("lwhttp/0.0.1").build();
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	auto ret = client->send(request, response);
	if ((response.getStatusCode() == HttpStatus::OK) && (response.getBodyLength() > 0))
	{
		auto data = response.getResponseBody()->getContent();
		auto len = response.getResponseBody()->getBodyLength();
		std::cout << "response length " << len << ", data:\n" << data << std::endl;
	}
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), HttpStatus::OK);
}

TEST(RequestTests, PostMethod)
{
	URL url("http://10.0.0.6/demo.php?id=1");
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().userAgent("lwhttp/0.0.1").build();
	const char json[] = R"({"id": 123, "value": "this is a string"})";
	std::shared_ptr<HttpBody> body = std::make_shared<HttpBodyImpl>(json, sizeof(json));
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).POST(body).build();
	request.header.setField("Content-Type", "application/json");
	HttpResponse response{};
	auto ret = client->send(request, response);
	if ((response.getStatusCode() == HttpStatus::OK) && (response.getBodyLength() > 0))
	{
		auto data = response.getResponseBody()->getContent();
		auto len = response.getResponseBody()->getBodyLength();
		std::cout << "response length " << len << ", data:\n" << data << std::endl;
	}
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), HttpStatus::OK);
}

TEST(RequestTests, PutMethod)
{
	URL url("http://10.0.0.6/demo.php?id=1");
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().userAgent("lwhttp/0.0.1").build();
	const char json[] = R"({"id": 123, "value": "this is a string"})";
	std::shared_ptr<HttpBody> body = std::make_shared<HttpBodyImpl>(json, sizeof(json));
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).PUT(body).build();
	request.header.setField("Content-Type", "application/json");
	HttpResponse response{};
	auto ret = client->send(request, response);
	if ((response.getStatusCode() == HttpStatus::OK) && (response.getBodyLength() > 0))
	{
		auto data = response.getResponseBody()->getContent();
		auto len = response.getResponseBody()->getBodyLength();
		std::cout << "response length " << len << ", data:\n" << data << std::endl;
	}
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), HttpStatus::OK);
}

TEST(RequestTests, DeleteMethod)
{
#ifdef DELETE
#undef DELETE
#endif
	URL url("http://10.0.0.6/demo.php?id=1");
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().userAgent("lwhttp/0.0.1").build();
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).DELETE().build();
	HttpResponse response{};
	auto ret = client->send(request, response);
	if ((response.getStatusCode() == HttpStatus::OK) && (response.getBodyLength() > 0))
	{
		auto data = response.getResponseBody()->getContent();
		auto len = response.getResponseBody()->getBodyLength();
		std::cout << "response length " << len << ", data:\n" << data << std::endl;
	}
	EXPECT_GT(ret, 0);
	EXPECT_EQ(response.getStatusCode(), HttpStatus::OK);
}

TEST(ClientTest, MultiRequestTest)
{
	multiRequestTest();
}
