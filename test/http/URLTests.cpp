#include <iostream>
#include <vector>
#include "include/http/URL.h"

int url_test()
{
	std::cout << "URL test..." << std::endl;
	std::vector<std::string> URLs{
			//username:password
			R"(http://username:password@www.github.com:8080/directory/file?query#ref)",
			R"(http://username:password@www.github.com/directory/file?query#ref)",
			R"(http://username:password@www.github.com:8080/directory/file)",
			R"(https://username:password@www.github.com:8080/directory/file?query#ref)",
			R"(https://username:password@www.github.com/directory/file?query#ref)",
			R"(https://username:password@www.github.com:8080/directory/file)",
			R"(https://username:password@www.github.com/)",
			//no authinfo
			R"(http://www.github.com:8080/directory/file?query#ref)",
			R"(http://www.github.com/directory/file?query#ref)",
			R"(http://www.github.com:8080/directory/file)",
			R"(https://www.github.com:8080/directory/file?query#ref)",
			R"(https://www.github.com/directory/file?query#ref)",
			R"(https://www.github.com:8080/directory/file)",
			R"(https://www.github.com/)",
			//token
			R"(http://token_abcd_1234@www.github.com:8080/directory/file?query#ref)",
			R"(http://token_abcd_1234@www.github.com/directory/file?query#ref)",
			R"(http://token_abcd_1234@www.github.com:8080/directory/file)",
			R"(https://token_abcd_1234@www.github.com:8080/directory/file?query#ref)",
			R"(https://token_abcd_1234@www.github.com/directory/file?query#ref)",
			R"(https://token_abcd_1234@www.github.com:8080/directory/file)",
			R"(https://token_abcd_1234@www.github.com/)",
	};

	size_t passed = 0, failed = 0, total = URLs.size();
	for (auto &item: URLs)
	{
		URL url(item);
		auto tmp = url.serialize();
		std::cout << item << ": " << ((item == url.serialize()) ? "pass" : "failed") << std::endl;
		if (item == url.serialize())
		{
			++passed;
		}
		else
		{
			++failed;
		}
	}
	std::cout << "Test summary: " << passed << "/" << total << std::endl;
	return 0;
}