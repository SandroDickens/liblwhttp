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

	virtual ~HttpBase() = default;
};

enum class HttpVersion
{
	HTTP_1_0,
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

	[[nodiscard]] std::string toString() const override;

	[[nodiscard]] std::string getField(const std::string &name) const;

	void setField(const std::string &name, const std::string &value);

private:
	std::map<std::string, std::string> fieldsMap;
};

/************************** HttpBody *************************/
class HttpBody
{
public:
	[[nodiscard]] virtual size_t getBodyLength() const = 0;

	virtual void setBodyLength(size_t len) = 0;

	[[nodiscard]] virtual const char *getContent() const = 0;

	virtual ~HttpBody() = default;
};

/************************ HttpBodyImpl ***********************/
class HttpBodyImpl : public HttpBody
{
public:

	HttpBodyImpl(const char *data, size_t len)
	{
		contentPtr = nullptr;
		bodyLength = 0;
		contentPtr = new char[len];
		if (contentPtr != nullptr)
		{
			bodyLength = len;
			memcpy(contentPtr, data, len);
		}
	}

	HttpBodyImpl(const HttpBodyImpl &other)
	{
		try
		{
			this->contentPtr = new char[other.bodyLength];
			this->bodyLength = other.bodyLength;
			memcpy(this->contentPtr, other.contentPtr, bodyLength);
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
				this->contentPtr = new char[other.bodyLength];
				this->bodyLength = other.bodyLength;
				memcpy(this->contentPtr, other.contentPtr, bodyLength);
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
		this->bodyLength = other.bodyLength;
		this->contentPtr = other.contentPtr;
		other.bodyLength = 0;
		other.contentPtr = nullptr;
	}

	HttpBodyImpl &operator=(HttpBodyImpl &&other) noexcept
	{
		if (this != &other)
		{
			this->bodyLength = other.bodyLength;
			this->contentPtr = other.contentPtr;
			other.bodyLength = 0;
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

	[[nodiscard]] size_t getBodyLength() const override
	{
		return bodyLength;
	}

	void setBodyLength(size_t len) override
	{
		bodyLength = len;
	}

	[[nodiscard]] const char *getContent() const override
	{
		return contentPtr;
	}

private:
	size_t bodyLength;
	char *contentPtr = nullptr;
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
