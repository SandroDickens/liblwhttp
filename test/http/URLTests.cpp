#include <vector>

#include <gtest/gtest.h>

#include <http/lwhttp.h>

int urlTests()
{
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

	for (auto &item: URLs)
	{
		URL url(item);
		auto serial = url.serialize();
		EXPECT_EQ(item, url.serialize());
	}
	return 0;
}

TEST(URLTests, serialize)
{
	urlTests();
}
