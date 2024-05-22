#include <Geode/Geode.hpp>
using namespace geode::prelude;

auto default_token = "Z2hwX3l1aTNPemtNMXhFSDdtS2RwbGpLeG8yRVU2N1dxczM3bUd3Wg==";

#define ghapiauth \
.userAgent(Mod::get()->getID()) \
.header("X-GitHub-Api-Version", "2022-11-28") \
.header("Authorization", fmt::format("Bearer {}", ZipUtils::base64URLDecode(default_token)))

$on_mod(Loaded) {
    //tryLoadAuthData();
}