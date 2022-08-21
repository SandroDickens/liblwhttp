# liblwhttp

## 1. 简介

liblwhttp(lightweight http library)是一个简单的HTTP客户端库, 支持HTTP和HTTPS请求, 支持Windows和Linux系统  
其中TLS依赖于OpenSSL

- Windows  
  在Windows系统下, liblwhttp编译得到的是静态库liblwhttp.lib(因为dll需要导出, 这对C++不友好)
- Linux  
  在Linux系统下, liblwhttp编译得到的是动态库liblwhttp.so

## 2. Wiki
[Wiki](../../wiki)
## 3. 编译

```shell
$ git clone https://github.com/YukPingFong/liblwhttp.git
# Debug, 如果开启ENABLE_TESTS, 务必关闭ENABLE_ASAN
$ cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON -DENABLE_ASAN=OFF
# Release
$ cmake -S . -B build-release -DENABLE_TESTS=ON
$ cd build-debug
$ make -j12
$ tree ./build-debug/bin/
./build-debug/bin/
├── demo
├── http_test
└── liblwhttp.so

0 directories, 3 files
$ ./bin/demo
```

## 4. 用法
### 1. 创建URL
```c++
URL url("https://www.google.com/");
```
### 2. 创建HttpRequest
```c++
HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
```
### 3. 创建HttpClient
```c++
std::shared_ptr<HttpClient> httpClient = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent("lwhttp/0.0.1").build();
```
### 4. 发生HTTP请求
```c++
HttpResponse response{};
httpClient->send(request, response);
```
### 5. 完整例子
#### a. CMake
CMakeLists.txt:
```cmake
add_executable(${TARGET_NAME} main.cpp)

include(FetchContent)
FetchContent_Declare(
        lwhttp
        GIT_REPOSITORY https://github.com/YukPingFong/liblwhttp.git
        GIT_TAG 554f215ccafcbdee2a3b4703c884e93afa5b14e8
)

FetchContent_MakeAvailable(lwhttp)

target_link_libraries(${TARGET_NAME} lwhttp)
```
#### b. 示例代码
```c++
#include <memory>
#include <iostream>

#include <http/lwhttp.h>

int main()
{
	std::shared_ptr<HttpClient> client = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent(
			"lwhttp/0.0.1").build();
	URL url("https://www.lwhttpd.org/demo/");
	HttpRequest request = HttpRequestBuilder::newBuilder().url(url).GET().build();
	HttpResponse response{};
	if (0 < client->send(request, response))
	{
		std::cout << "status: " << response.getStatusCode() << "/" << response.getReason() << std::endl;
		auto data = response.getResponseBody()->getContent();
		auto len = response.getResponseBody()->getBodyLength();
		std::cout << "response length " << len << ", data:\n" << data << std::endl;
	}
	else
	{
		std::cerr << "HTTP request send failed!" << std::endl;
	}

	return 0;
}
```
输出:
```text
$ ./demo
status: 200/OK
response capability 348, data:
<!DOCTYPE html>
<html lang="en">
<head>
    <style>
        h1 {
            text-align: center;
        }

        p {
            text-align: center;
        }

        div {
            text-align: center;
        }
    </style>
    <title>HTTP Test</title>
</head>
<body>
<h1>This is an HTTP test website</h1>
<p>liblwhttp</p>
</body>
</html>
```
