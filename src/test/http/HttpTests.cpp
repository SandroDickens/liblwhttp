#include <iterator>
#include <iostream>
#include "http/HttpBase.h"
#include "http/HttpRequest.h"

int main()
{
	std::cout << "URL test..." << std::endl;
	std::string URLs[] = {
			R"(http://username:password@www.github.com:8080/directory/file?query#ref)",
			R"(http://username:password@www.github.com/directory/file?query#ref)",
			R"(http://username:password@www.github.com:8080/directory/file)",
			R"(https://username:password@www.github.com:8080/directory/file?query#ref)",
			R"(https://username:password@www.github.com/directory/file?query#ref)",
			R"(https://username:password@www.github.com:8080/directory/file)"
	};

	for (const auto &URL: URLs)
	{
		class URL uri(URL);
		std::cout << URL << ": " << ((URL == uri.toString()) ? "pass" : "failed") << std::endl;
	}
	return 0;
}