#include "catch.hpp"
#include "jhc/win_shellink.hpp"

#ifdef JHC_WIN
TEST_CASE("Shellink1", "load") {
    jhc::WinShellink winShellLink;
    jhc::WinShellink::ShellinkErr err = winShellLink.load(R"(.\win_lnks\Postman.lnk)");
    REQUIRE(err == jhc::WinShellink::ShellinkErr::SHLLINK_ERR_NONE);
}
#endif