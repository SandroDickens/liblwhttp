# liblwhttp
## 简介
liblwhttp(lightweight http library)是一个简单的HTTP客户端库, 支持HTTP和HTTPS请求, 支持Windows和Linux系统  
其中TLS依赖于[LibreSSL](https://www.libressl.org/)  
- Windows  
在Windows系统下, liblwhttp编译得到的是静态库liblwhttp.lib(因为dll需要导出, 这对C++不友好)
- Linux  
在Linux系统下, liblwhttp编译得到的是动态库liblwhttp.so
## 编译
```shell
$ git clone https://github.com/YukPingFong/liblwhttp.git
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
$ cd build
$ make -j12
$ tree ./build/bin/
./build/bin/
├── demo
├── http_test
└── liblwhttp.so

0 directories, 3 files
$ ./bin/demo
```
## 用法
1. 创建URL
```c++
URL url("https://www.google.com/");
```
2. 创建HttpRequest
```c++
std::unique_ptr<HttpRequestBuilder> requestBuilder(HttpRequest::newBuilder());
HttpRequest request = requestBuilder->GET()->url(url)->build();
```
3. 创建HttpClient
```c++
std::shared_ptr<HttpClient> httpClient = HttpClientBuilder::newBuilder().redirect(Redirect::NORMAL).userAgent("lwhttp/0.0.1").build();
```
4. 发生HTTP请求
```c++
HttpResponse response;
httpClient->send(request, response);
std::cout << "Status Code: " << response.getStatusCode()
          << "\nReason: " << response.getReason()
          << "\nContentType: " << response.getContentType()
          << "\nBody Length: " << response.getBodyLength() << std::endl;
auto header = response.getHeader();
std::cout << "Header:\n" << header.toString() << std::endl;
auto bodyLength = response.getBodyLength();
if (bodyLength > 0)
{
    std::vector<char> body(bodyLength + 1);
    memcpy(body.data(), response.getResponseBody()->getContent(), bodyLength);
    body[bodyLength] = '\0';
    std::string bodyStr(body.data());
    std::cout << bodyStr;
}
```