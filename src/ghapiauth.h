#include <Geode/Geode.hpp>
using namespace geode::prelude;

inline static matjson::Value AUTH_DATA;

#define ghapiauth \
.userAgent(AUTH_DATA["name"].as_string()) \
.header("X-GitHub-Api-Version", "2022-11-28") \
//.header("Authorization", fmt::format("Bearer {}", AUTH_DATA["address"].as_string()))

$on_mod(Loaded) {
    //tryLoadAuthData();
}