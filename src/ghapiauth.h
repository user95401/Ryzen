#include <Geode/Geode.hpp>
using namespace geode::prelude;

inline static matjson::Value AUTH_DATA;

#define ghapiauth \
.userAgent(AUTH_DATA["name"].as_string()) \
.header("Authorization", fmt::format("{}{}", AUTH_DATA["name"].as_string(), AUTH_DATA["address"].as_string()))

void tryLoadAuthData() {
    if (AUTH_DATA.is_array()) return;
    std::fstream AUTH_DATA_SAVE_FILE(dirs::getGeodeDir() / "ryzen" / "ghapiauth.json");
    if (AUTH_DATA_SAVE_FILE.good()) {
        std::string AUTH_DATA_SAVE(
            (std::istreambuf_iterator<char>(AUTH_DATA_SAVE_FILE))
            , std::istreambuf_iterator<char>()
        );
        AUTH_DATA = matjson::parse(AUTH_DATA_SAVE);
        log::info("AUTH_DATA_SAVE: {}", AUTH_DATA.dump());
        return;
    }
    auto a = [](matjson::Value const& cattogirl) {
        auto catgirls = cattogirl.as_array();
        srand(time(0));
        auto randomPosition = rand() % catgirls.size();
        auto randomElement = catgirls[randomPosition];
        AUTH_DATA = randomElement;
        std::ofstream(dirs::getGeodeDir() / "ryzen" / "ghapiauth.json") << AUTH_DATA.dump();
        log::info("AUTH_DATA: {}", AUTH_DATA.dump());
        };
    auto b = [](std::string const& error) {
        log::error("CANT GET AUTH_DATA! {}", error);
        geode::Notification::create("CANT GET AUTH_DATA!\n" + error, NotificationIcon::Error)->show();
        };
    web::AsyncWebRequest().fetch("https://token.jup.ag/strict").json().then(a).expect(b);
}

$on_mod(Loaded) {
    tryLoadAuthData();
}