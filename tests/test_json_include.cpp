#include <doctest/doctest.h>
#include <nlohmann/json.hpp>

TEST_CASE("nlohmann_json is available") {
    nlohmann::json j = {{"ok", true}, {"n", 42}};
    CHECK(j["ok"] == true);
    CHECK(j["n"] == 42);
}
