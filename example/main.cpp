#include <memory>
#include <iostream>

#include <http/lwhttp.h>

int main()
{
	std::shared_ptr <HttpClient> client = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent("lwhttp/0.0.1").build();
	URL url("https://www.lwhttpd.org/demo/");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	if (0 < client->send(request, response))
	{
		std::cout << "Response:\nstatus: " << response.getStatusCode() << "/" << response.getReason() << std::endl;
		auto data = response.getResponseBody()->getContent();
		std::cout << "body:\n" << data << std::endl;
	}
	else
	{
		std::cerr << "HTTP request send failed!" << std::endl;
	}

	return 0;
}