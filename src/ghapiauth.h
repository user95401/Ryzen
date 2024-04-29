#include <Geode/Geode.hpp>
using namespace geode::prelude;

inline static matjson::Value AUTH_DATA;

#define ghapiauth \
.userAgent(AUTH_DATA["name"].as_string()) \
.header("Authorization", fmt::format("{}{}", AUTH_DATA["name"].as_string(), AUTH_DATA["address"].as_string()))

$on_mod(Loaded) {
    if (AUTH_DATA.is_array()) return;
    web::AsyncWebRequest()
        .fetch("https://token.jup.ag/strict")
        .json()
        .then([](matjson::Value const& cattogirl) {
        auto catgirls = cattogirl.as_array();
        srand(time(0));
        auto randomPosition = rand() % catgirls.size();
        auto randomElement = catgirls[randomPosition];
        AUTH_DATA = randomElement;
        log::info("AUTH_DATA: {}", AUTH_DATA.dump());
            })
        .expect([](std::string const& error) {
                log::error("CANT GET AUTH_DATA! {}", error);
                geode::Notification::create("CANT GET AUTH_DATA!\n" + error, NotificationIcon::Error)->show();
            });
}