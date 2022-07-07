#include "catch.hpp"
#include "jhc/win_http.hpp"
#include "jhc/md5.hpp"

TEST_CASE("Http1", "get") {
    jhc::WinHttp http;
    REQUIRE(http.openSession());
    REQUIRE(http.openConnect(L"https://www.baidu.com"));
    REQUIRE(http.openRequest());
    REQUIRE(http.sendRequest());
    REQUIRE(http.receiveResponse());
    std::vector<unsigned char> rspBody = http.getResponseBody();
    REQUIRE(rspBody.size() > 1000);
}

TEST_CASE("Http2", "download") {
    jhc::WinHttp http;
    REQUIRE(http.openSession());
    REQUIRE(http.openConnect(L"https://dlie.sogoucdn.com/se/sogou_explorer_11.0.1.34700_0000.exe"));
    REQUIRE(http.openRequest());
    REQUIRE(http.sendRequest());
    REQUIRE(http.receiveResponse());
    REQUIRE(http.saveResponseBodyToFile(L"C:\\sogou_explorer_11.0.1.34700_0000.exe"));
    REQUIRE(jhc::MD5::GetFileMD5(L"C:\\sogou_explorer_11.0.1.34700_0000.exe") == "73a0e33385b7fd3c2ce6279f35ef0c0b");
}