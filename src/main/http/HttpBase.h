#ifndef LWHTTPD_HTTPBASE_H
#define LWHTTPD_HTTPBASE_H

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


class HttpBase
{
public:
	[[nodiscard]] virtual std::string toString() const = 0;

	virtual void fromString(const std::string &str) = 0;
};

enum class HttpVersion
{
	HTTP_1_1,
	HTTP2
};

enum class Redirect
{
	/* Never redirect */
	NEVER,
	/* Always redirect*/
	ALWAYS,
	/* Always redirect, except from HTTPS URLs to HTTP URLs */
	NORMAL
};

enum class HttpMethod
{
	GET,
	POST,
	PUT,
	DELETE
};

/************************* HttpHeader ************************/
class HttpHeader : public HttpBase
{
public:
	void fromString(const std::string &str) override;

	std::string toString() const override;

	std::string getField(const std::string &name) const;

	void setField(const std::string &name, const std::string &value);

private:
	std::map<std::string, std::string> fieldsMap;
};

/************************** HttpBody *************************/
class HttpBody
{
public:
	[[nodiscard]] virtual unsigned long getContentLength() const = 0;

	[[nodiscard]] virtual const void *getContent() const = 0;

	virtual ~HttpBody() = default;
};

/************************ HttpBodyImpl ***********************/
class HttpBodyImpl : public HttpBody
{
public:

	HttpBodyImpl(const void *data, unsigned long len)
	{
		contentPtr = nullptr;
		contentLength = 0;
		contentPtr = new char[len];
		if (contentPtr != nullptr)
		{
			contentLength = len;
			memcpy(contentPtr, data, len);
		}
	}

	HttpBodyImpl(const HttpBodyImpl &other)
	{
		try
		{
			this->contentPtr = new unsigned char[other.contentLength];
			this->contentLength = other.contentLength;
			memcpy(this->contentPtr, other.contentPtr, contentLength);
		}
		catch (std::bad_alloc &e)
		{
			std::cout << e.what() << std::endl;
			throw e;
		}
	}

	HttpBodyImpl &operator=(const HttpBodyImpl &other)
	{
		if (this != &other)
		{
			try
			{
				this->contentPtr = new unsigned char[other.contentLength];
				this->contentLength = other.contentLength;
				memcpy(this->contentPtr, other.contentPtr, contentLength);
			}
			catch (std::bad_alloc &e)
			{
				std::cout << e.what() << std::endl;
				throw e;
			}
		}
		return *this;
	}

	HttpBodyImpl(HttpBodyImpl &&other) noexcept
	{
		this->contentLength = other.contentLength;
		this->contentPtr = other.contentPtr;
		other.contentLength = 0;
		other.contentPtr = nullptr;
	}

	HttpBodyImpl &operator=(HttpBodyImpl &&other) noexcept
	{
		if (this != &other)
		{
			this->contentLength = other.contentLength;
			this->contentPtr = other.contentPtr;
			other.contentLength = 0;
			other.contentPtr = nullptr;
		}
		return *this;
	}

	~HttpBodyImpl() override
	{
		if (contentPtr != nullptr)
		{
			free(contentPtr);
		}
	}

	[[nodiscard]] unsigned long getContentLength() const override
	{
		return contentLength;
	}

	[[nodiscard]] const void *getContent() const override
	{
		return contentPtr;
	}

private:
	unsigned long contentLength;
	void *contentPtr = nullptr;
};

/************************** Cookie ***************************/
class Cookie : public HttpBase
{
public:
	void setCookie(const std::string &name, std::string &value);

	std::string getCookie(const std::string &name);

	std::vector<std::pair<std::string, std::string>> getCookies();

	bool hasCookie(const std::string &name);

private:
	std::unordered_map<std::string, std::string> cookiesMap;
};

void toUpCase(std::string &str);

void toLowCase(std::string &str);

void toUpCamelCase(std::string &str);

#endif //LWHTTPD_HTTPBASE_H
