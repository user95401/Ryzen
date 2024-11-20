#include <_main.hpp>
#include <Geode/utils/web.hpp>

namespace github {

    inline web::WebRequest web_request();

    class User {
    public:

        std::string m_login = "no login";
        intmax_t m_id = -1;
        std::string m_node_id = "";
        std::string m_avatar_url = "";
        intmax_t m_gravatar_id = -1;
        std::string m_url = "";
        std::string m_html_url = "";
        std::string m_followers_url = "";
        std::string m_following_url = "";
        std::string m_gists_url = "";
        std::string m_starred_url = "";
        std::string m_subscriptions_url = "";
        std::string m_organizations_url = "";
        std::string m_repos_url = "";
        std::string m_events_url = "";
        std::string m_received_events_url = "";
        std::string m_type = "";
        std::string m_user_view_type = "";
        std::string m_name = "unnameed";
        std::string m_company = "";
        std::string m_blog = "";
        std::string m_location = "";
        std::string m_email = "";
        std::string m_bio = "";

        matjson::Value m_json;

        User(matjson::Value json = matjson::Value()) {
            m_json = json;

            m_login = json["login"].asString().unwrapOr(m_login);
            m_id = json["id"].asInt().unwrapOr(m_id);
            m_node_id = json["node_id"].asString().unwrapOr(m_node_id);
            m_avatar_url = json["avatar_url"].asString().unwrapOr(m_avatar_url);
            m_gravatar_id = json["gravatar_id"].asInt().unwrapOr(m_gravatar_id);
            m_url = json["url"].asString().unwrapOr(m_url);
            m_html_url = json["html_url"].asString().unwrapOr(m_html_url);
            m_followers_url = json["followers_url"].asString().unwrapOr(m_followers_url);
            m_following_url = json["following_url"].asString().unwrapOr(m_following_url);
            m_gists_url = json["gists_url"].asString().unwrapOr(m_gists_url);
            m_starred_url = json["starred_url"].asString().unwrapOr(m_starred_url);
            m_subscriptions_url = json["subscriptions_url"].asString().unwrapOr(m_subscriptions_url);
            m_organizations_url = json["organizations_url"].asString().unwrapOr(m_organizations_url);
            m_repos_url = json["repos_url"].asString().unwrapOr(m_repos_url);
            m_events_url = json["events_url"].asString().unwrapOr(m_events_url);
            m_received_events_url = json["received_events_url"].asString().unwrapOr(m_received_events_url);
            m_type = json["type"].asString().unwrapOr(m_type);
            m_user_view_type = json["user_view_type"].asString().unwrapOr(m_user_view_type);
            m_name = json["name"].asString().unwrapOr(m_name);
            m_company = json["company"].asString().unwrapOr(m_company);
            m_blog = json["blog"].asString().unwrapOr(m_blog);
            m_location = json["location"].asString().unwrapOr(m_location);
            m_email = json["email"].asString().unwrapOr(m_email);
            m_bio = json["bio"].asString().unwrapOr(m_bio);
        };
        ~User() {}

    };

    namespace account {

        inline auto user = User();

        inline void try_load_user() {
            if (user.m_id > 0) return;
            auto req = github::web_request();
            auto listener = new EventListener<web::WebTask>;
            listener->bind(
                [](web::WebTask::Event* e) {
                    if (web::WebResponse* res = e->getValue()) {
                        auto json = res->json();
                        auto string = res->string();
                        if (json.unwrapOrDefault().contains("id")) user = json.unwrapOrDefault();
                        else if (SETTING(bool, "Auth Warn")) {
                            //Account Authorization Warn?
                        }
                    }
                }
            );
            listener->setFilter(req.send(
                "GET",
                "https://api.github.com/user"
            ));
        }

        inline std::string get_token() {
            return Mod::get()->getSavedValue<std::string>("gh_access_token");
        }

        inline void set_token(std::string token) {
            Mod::get()->setSavedValue("gh_access_token", token);
            Mod::get()->saveData();
            user = matjson::Value();
            try_load_user();
        }

        inline bool has_token() {
            return (get_token().size() > 3);
        }

        inline web::WebRequest get_basic_web_request() {
            auto req = web::WebRequest();
            req.userAgent(Mod::get()->getID());
            req.header("X-GitHub-Api-Version", "2022-11-28");
            if (has_token()) req.header("Authorization", fmt::format("Bearer {}", get_token()));
            return req;
        }

        inline web::WebRequest* create_basic_web_request() {
            auto req = new web::WebRequest();
            req->userAgent(Mod::get()->getID());
            req->header("X-GitHub-Api-Version", "2022-11-28");
            if (has_token()) req->header("Authorization", fmt::format("Bearer {}", get_token()));
            return req;
        }

        $on_mod(Loaded) {
            account::try_load_user();
        }

    };

    inline web::WebRequest web_request() {
        auto req = web::WebRequest();
        req.userAgent(Mod::get()->getID());
        req.header("X-GitHub-Api-Version", "2022-11-28");
        if (account::has_token()) req.header("Authorization", fmt::format("Bearer {}", account::get_token()));
        return req;
    }

}

inline void rznMenuMusic(CCLayer* rtn = nullptr) {
    if (SETTING(bool, "rzn_menu_music")) GameManager::sharedState()->fadeInMusic(
        "Fishbone.mp3" ""_spr
    );
};

class RznLayer : public CCLayer {
public:
    static auto create(std::function<void()> onBack) {

        auto rtn = new RznLayer;
        rtn->init();

        rznMenuMusic();
        {
            //setup
            rtn->setKeypadEnabled(true);
            rtn->setTouchEnabled(true);
            auto backgroundSprite = CCSprite::create("GJ_gradientBG.png");
            backgroundSprite->setScaleX(rtn->getContentSize().width / backgroundSprite->getContentSize().width);
            backgroundSprite->setScaleY(rtn->getContentSize().height / backgroundSprite->getContentSize().height);
            backgroundSprite->setAnchorPoint({ 0.5f, 0.5f });
            backgroundSprite->setPosition(rtn->getContentSize() / 2);
            backgroundSprite->setColor({ 120, 120, 130 });
            auto baseBgCol = backgroundSprite->getColor();
            backgroundSprite->runAction({
                CCRepeatForever::create(CCSequence::create(
                        CCTintTo::create(0.0f, baseBgCol.r + -2, baseBgCol.g + -2, baseBgCol.b + 0),
                        CCDelayTime::create(0.01f),
                        CCTintTo::create(0.0f, baseBgCol.r + 0, baseBgCol.g + 0, baseBgCol.b + 0),
                        CCDelayTime::create(0.1f),
                        CCTintTo::create(0.0f, baseBgCol.r + 1, baseBgCol.g + 1, baseBgCol.b + 1),
                        CCDelayTime::create(0.01f),
                        CCTintTo::create(0.0f, baseBgCol.r + 5, baseBgCol.g + 5, baseBgCol.b + 0),
                        CCDelayTime::create(0.01f),
                        CCTintTo::create(0.0f, baseBgCol.r + 2, baseBgCol.g + 2, baseBgCol.b + -1),
                        CCDelayTime::create(0.10f),
                        nullptr
                    ))
                });
            backgroundSprite->runAction(CCRepeatForever::create(CCShaky3D::create(10.0f, CCSizeMake(1, 10), 1, 0)));
            rtn->addChild(backgroundSprite, -10);
            //a
            auto doting = CCSprite::create("doting.png"_spr);
            doting->setAnchorPoint(CCPoint());
            doting->setScaleX((rtn->getContentSize().width / doting->getContentSize().width));
            doting->setScaleY((rtn->getContentSize().height / doting->getContentSize().height));
            doting->runAction(CCRepeatForever::create(CCShaky3D::create(5.0f, CCSizeMake(5, 5), 50, 0)));
            rtn->addChild(doting, -10, 5931);
            /*SquareShadowCorners*/ {
                auto scale = 1.f;
                auto opacity = 160;
                //SquareShadowCorner1
                auto SquareShadowCorner1 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                SquareShadowCorner1->setScaleX(scale);
                SquareShadowCorner1->setOpacity(opacity);
                SquareShadowCorner1->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner1->getContentSize().height);
                SquareShadowCorner1->setAnchorPoint({ 0, 0 });
                rtn->addChild(SquareShadowCorner1, -9);
                //SquareShadowCorner2
                auto SquareShadowCorner2 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                SquareShadowCorner2->setScaleX(-scale);
                SquareShadowCorner2->setOpacity(opacity);
                SquareShadowCorner2->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner2->getContentSize().height);
                SquareShadowCorner2->setAnchorPoint({ 0, 0 });
                SquareShadowCorner2->setPositionX(CCDirector::sharedDirector()->getScreenRight());
                rtn->addChild(SquareShadowCorner2, -9);
            };
            /*gauntletCorners*/ {
                //gauntletCorner_001
                auto gauntletCorner_001 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
                gauntletCorner_001->setPosition({ 0, 0 });
                gauntletCorner_001->setRotation(0);
                gauntletCorner_001->setAnchorPoint({ 0.f,0.f });
                rtn->addChild(gauntletCorner_001);//add gauntletCorner_001
                //gauntletCorner_002
                auto gauntletCorner_002 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
                gauntletCorner_002->setPosition({
                    CCDirector::sharedDirector()->getWinSize().width,
                    0.f
                    });
                gauntletCorner_002->setScaleX(-1.f);
                gauntletCorner_002->setAnchorPoint({ 0.f,0.f });
                rtn->addChild(gauntletCorner_002);//add gauntletCorner_002
                //gauntletCorner_003
                auto gauntletCorner_003 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
                gauntletCorner_003->setPosition({
                    (CCDirector::sharedDirector()->getWinSize().width),
                    (CCDirector::sharedDirector()->getWinSize().height)
                    });
                gauntletCorner_003->setRotation(180.f);
                gauntletCorner_003->setAnchorPoint({ 0.f,0.f });
                rtn->addChild(gauntletCorner_003);//add gauntletCorner_003
            };
            /*backBtn*/ {
                auto back = CCMenuItemExt::createSpriteExtra(
                    CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
                    [onBack](auto) { onBack(); }
                );
                back->setID("back");
                auto menuBack = CCMenu::createWithItem(back);
                menuBack->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
                rtn->addChild(menuBack);
            };
        };

        return rtn;

    }
    virtual void keyBackClicked() override {
        if (auto backBtn = typeinfo_cast<CCMenuItem*>(this->getChildByIDRecursive("back")))
            backBtn->activate();
    };
};

inline auto RznAuthLayer() {
    auto __this = RznLayer::create(
        []() {
            CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
        }
    );

    auto menu = CCMenu::create();
    __this->addChild(menu);



    menu->setLayout(AnchorLayout::create());

    return __this;
}

inline auto RznListLayer() {
    auto __this = RznLayer::create(
        []() {
            CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
            GameManager::get()->fadeInMenuMusic();
        }
    );

    auto menu = CCMenu::create();
    __this->addChild(menu);

    if (auto Ryzen_profileButton_001 = CCSprite::create("Ryzen_profileButton_001.png"_spr)) {
        Ryzen_profileButton_001->setScale(0.9f);
        auto profileButton = CCMenuItemExt::createSpriteExtra(Ryzen_profileButton_001, 
            [](auto) {
                log::debug("{}", github::account::user.m_json.dump());
                if (github::account::user.m_id > 0) void();
                else {
                    auto scene = CCScene::create();
                    scene->addChild(RznAuthLayer());
                    CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
                }
            }
        );
        profileButton->setID("profileButton");
        profileButton->setLayoutOptions(
            AnchorLayoutOptions::create()
            ->setAnchor(Anchor::BottomLeft)
            ->setOffset({ 33.f, 33.f })
        );
        menu->addChild(profileButton);
    }

    menu->setLayout(AnchorLayout::create());

    return __this;
}

class ModsLayer : public CCNode {};

#include <Geode/modify/CCLayer.hpp>
class $modify(ModsLayerExt, CCLayer) {
	void customSetup() {
		if (auto actions_menu = typeinfo_cast<CCMenu*>(getChildByIDRecursive("actions-menu"))) {

			auto image = CircleButtonSprite::createWithSprite(
				"Ryzen_LogoIcon_001.png"_spr, 1.0f, 
				!Loader::get()->getLoadedMod("geode.loader")->getSettingValue<bool>("enable-geode-theme") ? 
				CircleBaseColor::Green : CircleBaseColor::DarkPurple
			);
			image->setScale(0.8f);

			CCMenuItemSpriteExtra* rzn_openup_btn = CCMenuItemExt::createSpriteExtra(
				image, [](CCNode*) {
                    auto scene = CCScene::create();
                    scene->addChild(RznListLayer());
                    CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
				}
			);
			rzn_openup_btn->setID("rzn_openup_btn"_spr);
			actions_menu->addChild(rzn_openup_btn);
			actions_menu->updateLayout();

		}
	}
	bool init() {
		if (!CCLayer::init()) return false;
		if (typeinfo_cast<ModsLayer*>(this)) queueInMainThread([this] { customSetup(); });
		return true;
	}
};