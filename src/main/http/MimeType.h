#ifndef LWHTTPD_MIME_TYPES_H
#define LWHTTPD_MIME_TYPES_H


#include <unordered_map>
#include <string>

class MimeType
{
public:
	static std::string getMIMEType(const std::string &fileName);

	static bool isElf(const std::string &fileName);

	MimeType(const MimeType &obj) = delete;

	MimeType &operator=(const MimeType &) = delete;

	static MimeType &getInstance()
	{
		static MimeType instance;
		return instance;
	}

private:
	MimeType();

	static std::unordered_map<std::string, std::string> mimeMap;
};


#endif //LWHTTPD_MIME_TYPES_H
