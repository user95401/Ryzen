#include <_main.hpp>
#include <Geode/utils/web.hpp>

class WebImageNode : public CCNode {
public:
    static auto create(std::string url = "", CCSize size = { 30.f, 30.f }, std::function<void(CCSprite*)> loaded = [](auto) {}) {
        auto __this = new WebImageNode();
        __this->init();
        __this->setContentSize(size);

        //temp sprite
        auto temp = CCSprite::createWithSpriteFrameName("edit_eDamageSquare_001.png");
        temp->setAnchorPoint(CCPointZero);
        limitNodeSize(temp, size, 1337.f, 0.1f);
        temp->setID("temp");
        __this->addChild(temp);
        
        auto filep = dirs::getTempDir() / ("." + std::string(ZipUtils::base64URLEncode(url).c_str()));
        auto req = web::WebRequest();
        auto listener = new EventListener<web::WebTask>;
        listener->bind(
            [__this, loaded, temp, filep, size](web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {
                    if (res->code() < 399) {
                        res->into(filep);
                        if (temp) temp->setVisible(0);
                        auto sprite = CCSprite::create(filep.string().c_str());
                        sprite->setScale(size.width / sprite->getContentSize().width);
                        limitNodeSize(sprite, size, 1337.f, 0.1f);
                        sprite->setAnchorPoint(CCPointZero);
                        __this->addChild(sprite);
                        fs::remove(filep, *(new std::error_code()));
                        loaded(sprite);
                    }
                }
            }
        );
        listener->setFilter(req.send("GET", url));

        return __this;
    }
};

namespace github {

    inline web::WebRequest web_request();

    class user : public CCNode {
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

        static auto create(matjson::Value json = {}) {
            auto rtn = new user();
            rtn->init();
            rtn->update(json);
            return rtn;
        }

        void update(matjson::Value json) {
            m_json = json;
            m_login = m_json["login"].asString().unwrapOr(user().m_login);
            m_id = m_json["id"].asInt().unwrapOr(user().m_id);
            m_node_id = m_json["node_id"].asString().unwrapOr(user().m_node_id);
            m_avatar_url = m_json["avatar_url"].asString().unwrapOr(user().m_avatar_url);
            m_gravatar_id = m_json["gravatar_id"].asInt().unwrapOr(user().m_gravatar_id);
            m_url = m_json["url"].asString().unwrapOr(user().m_url);
            m_html_url = m_json["html_url"].asString().unwrapOr(user().m_html_url);
            m_followers_url = m_json["followers_url"].asString().unwrapOr(user().m_followers_url);
            m_following_url = m_json["following_url"].asString().unwrapOr(user().m_following_url);
            m_gists_url = m_json["gists_url"].asString().unwrapOr(user().m_gists_url);
            m_starred_url = m_json["starred_url"].asString().unwrapOr(user().m_starred_url);
            m_subscriptions_url = m_json["subscriptions_url"].asString().unwrapOr(user().m_subscriptions_url);
            m_organizations_url = m_json["organizations_url"].asString().unwrapOr(user().m_organizations_url);
            m_repos_url = m_json["repos_url"].asString().unwrapOr(user().m_repos_url);
            m_events_url = m_json["events_url"].asString().unwrapOr(user().m_events_url);
            m_received_events_url = m_json["received_events_url"].asString().unwrapOr(user().m_received_events_url);
            m_type = m_json["type"].asString().unwrapOr(user().m_type);
            m_user_view_type = m_json["user_view_type"].asString().unwrapOr(user().m_user_view_type);
            m_name = m_json["name"].asString().unwrapOr(user().m_name);
            m_company = m_json["company"].asString().unwrapOr(user().m_company);
            m_blog = m_json["blog"].asString().unwrapOr(user().m_blog);
            m_location = m_json["location"].asString().unwrapOr(user().m_location);
            m_email = m_json["email"].asString().unwrapOr(user().m_email);
            m_bio = m_json["bio"].asString().unwrapOr(user().m_bio);
        }

        void reload(std::function<void()> onFinish = [] {}, std::function<void()> onFault = [] {}) {
            auto req = github::web_request();
            auto listener = new EventListener<web::WebTask>;
            listener->bind(
                [this, onFinish, onFault](web::WebTask::Event* e) {
                    if (web::WebResponse* res = e->getValue()) {
                        auto json = res->json();
                        auto string = res->string();
                        if (json.unwrapOrDefault().contains("id")) {
                            this->update(json.unwrapOrDefault());
                            onFinish();
                        }
                        else {
                            onFault();
                        }
                    }
                }
            );
            listener->setFilter(req.send(
                "GET", "https://api.github.com/user"
            ));
        }

    };

    namespace account {

        inline auto user = github::user::create();

        inline void try_load_user() {
            if (user->m_id > 0) return;
            auto req = github::web_request();
            auto listener = new EventListener<web::WebTask>;
            listener->bind(
                [](web::WebTask::Event* e) {
                    if (web::WebResponse* res = e->getValue()) {
                        auto json = res->json();
                        auto string = res->string();
                        if (json.unwrapOrDefault().contains("id")) user->update(json.unwrapOrDefault());
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
            user->update({});
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

    geode::utils::web::openLinkInBrowser("https://user95401.github.io/Ryzen/auth.html");

    auto input_menu = CCMenu::create();
    __this->addChild(input_menu);

    //input
    auto input = TextInput::create(280.f, "the code");
    input->setID("input");
    input_menu->addChild(input);
    //paste
    auto paste = CCMenuItemExt::createSpriteExtra(
        CCLabelBMFont::create(
            "paste from clipboard", "chatFont.fnt"
        ), [](CCNode* btn) {
            auto menu = btn->getParent();
            auto input = menu ? typeinfo_cast<TextInput*>(menu->getChildByIDRecursive("input")) : nullptr;
            if (input) input->setString(utils::clipboard::read());
        }
    );
    paste->setPosition(CCPointMake(-140.f, -16.f));
    paste->setAnchorPoint(CCPointMake(0.f, 1.f));
    input_menu->addChild(paste);
    //FINISH
    auto FINISH = CCMenuItemExt::createSpriteExtra(
        CCLabelBMFont::create(
            "| FINISH |", "chatFont.fnt"
        ), [__this, input](CCObject* btnObj) {
            //code
            auto code = std::string("");
            if (input) code = input->getString();
            //req
            auto a = [__this](matjson::Value const& catgirl) {
                if (not catgirl.contains("access_token")) {
                    auto asd = geode::createQuickPopup(
                        "Failed getting token",
                        catgirl.dump(),
                        "Nah", nullptr, 420.f, nullptr, false
                    );
                    asd->show();
                    return;
                }
                github::account::set_token(catgirl["access_token"].asString().unwrapOrDefault());
                
                auto asd = geode::createQuickPopup(
                    "Access token saved!",
                    "Now u have no limits and able to create comments...",
                    "OK", nullptr,
                    [__this](auto, auto) {
                        if (__this) __this->keyBackClicked();
                    }
                );
                };
            auto b = [](std::string const& error)
                {// something went wrong with our web request Q~Q
                    auto message = error;
                    auto asd = geode::createQuickPopup(
                        "Request exception",
                        message,
                        "Nah", nullptr, 420.f, nullptr, false
                    );
                    asd->show();
                };
            auto req = web::WebRequest();
            auto listener = new EventListener<web::WebTask>;
            listener->bind(
                [a, b](web::WebTask::Event* e) {
                    if (web::WebResponse* res = e->getValue()) {
                        std::string data = res->string().unwrapOr("");
                        //json
                        auto json = matjson::parse(data);
                        auto json_val = json.unwrapOrDefault();
                        if (json.isErr()) return b("Error parsing JSON: " + json.unwrapErr().message);
                        //call the some stuff
                        if (res->code() < 399) a(json_val);
                        else b(data);
                    }
                }
            );
            req.header("Accept", "application/json");
            req.bodyString(
                fmt::format("code={}", code) +
                "&" "client_id=Ov23li0XhdQqNS3Bf3s4"
                "&" "client_secret=298dfebac04369ae19286d7b458a1143a7d51e72"
                //"&" ""
            );
            listener->setFilter(req.send("POST", "https://github.com/login/oauth/access_token"));
        }
    );
    FINISH->setPosition(CCPointMake(140.f, -16.f));
    FINISH->setAnchorPoint(CCPointMake(1.f, 1.f));
    FINISH->setColor(ccYELLOW);
    input_menu->addChild(FINISH);

    return __this;
}

inline RznLayer* RznUserLayer(github::user* user) {
    auto __this = RznLayer::create(
        []() {
            CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
        }
    );

    auto its_me = user->m_id == github::account::user->m_id;

    if (auto center_menu = CCMenu::create()) {

        __this->addChild(center_menu);

        auto right_menu = CCMenu::create();
        right_menu->setAnchorPoint(CCPointMake(0.f, 0.5f));
        right_menu->setContentSize(CCSizeMake(170.f, 0.f));
        center_menu->addChild(right_menu);

        auto avatar = WebImageNode::create(user->m_avatar_url, { 120.f, 120.f });
        avatar->setID("avatar");
        right_menu->addChild(avatar);

        auto text1 = std::stringstream();
        auto dot = "<c-DDDDDD>\\- </c>";
        text1 << "## [" << user->m_name << "]("<< user->m_html_url << ")" << std::endl;
        text1 << "<c-999999>" << user->m_login << ", id: " << user->m_id << "</c>" << std::endl << std::endl;
        if (user->m_bio.size() > 1) text1 << "`" << user->m_bio << "`" << std::endl << std::endl;
        if (user->m_json["followers"].isNumber()) text1 << dot << "<c-AAAAAA>Followers: </c>" << user->m_json["public_repos"].asInt().unwrapOrDefault();
        if (user->m_json["following"].isNumber()) text1 << ", <c-AAAAAA>Following: </c>" << user->m_json["public_gists"].asInt().unwrapOrDefault() << std::endl << std::endl;
        if (user->m_location.size() > 1) text1 << dot << "<c-AAAAAA>Location: </c>" << user->m_location << std::endl << std::endl;
        if (user->m_company.size() > 1) text1 << dot << "<c-AAAAAA>Company: </c>" << user->m_company << std::endl << std::endl;
        if (user->m_email.size() > 1) text1 << user->m_email << std::endl << std::endl;
        if (user->m_blog.size() > 1) text1 << user->m_blog << std::endl << std::endl;

        auto mdtext1 = MDTextArea::create(text1.str(), { right_menu->getContentWidth() + 12.f, 144.f });
        mdtext1->setID("mdtext1");
        mdtext1->setZOrder(0);
        public_cast(mdtext1, m_bgSprite)->removeAllChildrenWithCleanup(0);
        if (mdtext1->getScrollLayer()->m_contentLayer->getContentHeight() > mdtext1->getContentHeight()) {
            auto scrlbar = Scrollbar::create(mdtext1->getScrollLayer());
            scrlbar->setPosition(CCPointMake(-10.f, mdtext1->getContentHeight()/2));
            mdtext1->addChild(scrlbar);
        }
        else {
            mdtext1->getScrollLayer()->m_peekLimitTop = (0);
            mdtext1->getScrollLayer()->m_peekLimitBottom = (0);
        }
        right_menu->addChild(mdtext1);

        auto logout = CCMenuItemExt::createSpriteExtra(
            CCLabelBMFont::create(
                "| LOGOUT |", "chatFont.fnt"
            ), [__this](CCNode* btn) {
                github::account::set_token("logged_out");
                if (__this) __this->keyBackClicked();
            }
        );
        logout->setColor(ccRED);
        logout->setAnchorPoint(CCPointMake(0.f, 0.f));
        if (its_me) right_menu->addChild(logout);

        right_menu->setLayout(RowLayout::create()
            ->setGrowCrossAxis(1)
            ->setCrossAxisAlignment(AxisAlignment::End)
            ->setAxisAlignment(AxisAlignment::Start)
        );

        auto debug = MDTextArea::create(
            fmt::format("```\n{}\n```", github::account::user->m_json.dump()), { 280, 260 }
        );
        center_menu->addChild(debug);

        center_menu->setLayout(RowLayout::create());

    };

    if (auto menu = CCMenu::create()) {

        __this->addChild(menu);

        if (auto Ryzen_ReloadBtn_001 = CCSprite::create("Ryzen_ReloadBtn_001.png"_spr)) {
            Ryzen_ReloadBtn_001->setScale(0.9f);
            auto ReloadBtn = CCMenuItemExt::createSpriteExtra(Ryzen_ReloadBtn_001,
                [__this, user](auto) {
                    user->reload(
                        [__this, user] {
                            if (__this) if (auto scene = __this->getParent()) scene->addChild(RznUserLayer(user));
                            if (__this) __this->removeFromParent();
                        }
                    );
                }
            );
            ReloadBtn->setID("ReloadBtn");
            ReloadBtn->setLayoutOptions(
                AnchorLayoutOptions::create()
                ->setAnchor(Anchor::BottomRight)
                ->setOffset({ -33.f, 33.f })
            );
            menu->addChild(ReloadBtn);
        }

        menu->setLayout(AnchorLayout::create());

    };

    return __this;
}

inline auto RznListLayer() {
    auto __this = RznLayer::create(
        []() {
            CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
            GameManager::get()->fadeInMenuMusic();
        }
    );

    auto paddingx = 146.f;
    auto paddingt = 36.f;
    auto scroll_size = CCSize(CCDirector::get()->getScreenRight() - paddingx - 4, CCDirector::get()->getScreenTop() - paddingt);

    //pages stuff
    {
        //input label 
        auto label = CCLabelTTF::create("Page:", "arial", 16.f);
        label->CCNode::setPosition(36.f, 120.f);
        __this->addChild(label);
        //InputNode
        auto page_sv = Mod::get()->getSavedValue<std::string>("page");
        page_sv = page_sv.empty() ? "1" : page_sv;
        Mod::get()->setSavedValue<std::string>("page", page_sv);
        auto page = TextInput::create(
            38,
            "int",
            "chatFont.fnt"
        );
        page->setString(page_sv.c_str());
        page->getInputNode()->m_allowedChars = "0123456789";
        page->getInputNode()->setAllowedChars(page->getInputNode()->m_allowedChars);
        page->setID("page");
        page->setPosition(36.f, 90.f);
        __this->addChild(page);
        //arrows
        CCMenu* arrows = CCMenu::create();
        arrows->setID("arrows");
        __this->addChild(arrows);
        auto size = 1.5f;
        auto sizeMult = 3.f;
        CCMenuItemSpriteExtra* arrow_left = CCMenuItemExt::createSpriteExtra(
            CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"),
            [](auto) {

            }
        );
        arrow_left->setID("arrow_left");
        arrow_left->setSizeMult(sizeMult);
        arrow_left->getNormalImage()->setScale(size);
        arrows->addChild(arrow_left);
        CCMenuItemSpriteExtra* arrow_right = CCMenuItemExt::createSpriteExtra(
            CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"),
            [](auto) {

            }
        );
        arrow_right->setID("arrow_right");
        arrow_right->setSizeMult(sizeMult);
        arrow_right->getNormalImage()->setScale(size);
        arrows->addChild(arrow_right);
        arrows->alignItemsHorizontallyWithPadding(__this->getContentWidth() - 90.f);
    }
    //sort stuff
    {
        auto menu = CCMenu::create();
        __this->addChild(menu);
        menu->CCNode::setPosition(__this->getContentWidth() - 36, 90.f);
        //label 
        auto label = CCLabelTTF::create("Sort:", "arial", 16.f);
        label->CCNode::setPosition(0.f, 30.f);
        menu->addChild(label);
        //btn
        auto sort_sv = Mod::get()->getSavedValue<std::string>("sort");
        sort_sv = sort_sv.empty() ? "created" : sort_sv;
        Mod::get()->setSavedValue<std::string>("sort", sort_sv);
        auto sort = CCLabelTTF::create(
            sort_sv.c_str(),
            "arial", 10.f
        );
        sort->setID("sort");
        auto item = CCMenuItemExt::createSpriteExtra(
            sort,
            [](auto) {

            }
        );
        item->setID("switch_sort");
        item->m_colorEnabled = 1;
        item->m_animationEnabled = 0;
        item->setSizeMult(3.f);
        menu->addChild(item);
        //bg
        auto bg = CCScale9Sprite::create("square02_small.png");
        bg->setOpacity(75);
        bg->setContentSize(sort->getContentSize() + CCSize(16.f, 16.f));
        menu->addChild(bg, -2);
    }
    //filter
    {
        auto input = TextInput::create(
            scroll_size.width,
            "Filters...",
            "chatFont.fnt"
        );
        input->setString("");
        input->getInputNode()->m_filterSwearWords = false;
        input->getInputNode()->m_allowedChars = " !\"#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
        input->setFilter(input->getInputNode()->m_allowedChars);
        input->setID("input");
        input->getInputNode()->m_placeholderColor = ccColor3B(160, 160, 160);
        input->getInputNode()->m_placeholderLabel->setColor(input->getInputNode()->m_placeholderColor);
        input->setPositionX(CCDirector::get()->getScreenRight() / 2);
        input->setPositionY(CCDirector::get()->getScreenTop());
        input->setPositionY(input->getPositionY() - (input->getContentSize().height / 2));
        __this->addChild(input);
    }

    ScrollLayer* scroll;

    auto setupListStep4 = [__this, scroll, scroll_size](matjson::Value catgirls)
        {
            log::debug("catgirls: {}", catgirls.dump());
            auto scroll = dynamic_cast<ScrollLayer*>(__this->getChildByID("scroll"));
            if (not scroll) return;
            scroll->m_contentLayer->removeAllChildren();
            for (auto catgirl : catgirls) {
                scroll->m_contentLayer->addChild(TextInput::create(scroll_size.width, catgirl["title"].dump()));
            }
            scroll->m_contentLayer->setLayout(RowLayout::create()
                ->setGrowCrossAxis(1)
                ->setCrossAxisAlignment(AxisAlignment::End)
                ->setAxisAlignment(AxisAlignment::Start)
            );
        };

    auto setupListStep3 = [__this, setupListStep4](web::WebResponse* res)
        {
            //resp
            std::string data = res->string().unwrapOr("");
            //json
            auto parse = matjson::parse(data);
            auto json_val = parse.unwrapOrDefault();
            if (parse.isErr()) data = ("Error parsing JSON: " + parse.unwrapErr().message + " in :" + data);
            //a
            if (res->code() > 399) {
                auto message = data;
                auto parse = matjson::parse(message);
                auto json = parse.unwrapOrDefault();
                if (json.contains("message")) {
                    message = json["message"].asString().unwrapOrDefault();
                }
                auto asd = geode::createQuickPopup(
                    "Request exception",
                    message,
                    "Nah", nullptr, 420.f, nullptr, false
                );
                asd->m_scene = __this;
                asd->show();
            }
            else {
                // do something with the catgirls :3
                setupListStep4(json_val);
                auto local_issues_save = getMod()->getSaveDir() / "issues.json";
                fs::create_directories(local_issues_save.parent_path(), fs::last_err_code);
                std::ofstream(local_issues_save.string().c_str()) << json_val.dump();
            }
        };

    auto setupListStep2 = [__this, scroll, setupListStep3]()
        {
            auto per_page = Mod::get()->getSettingValue<int64_t>("per_page");
            auto issues_repo = Mod::get()->getSettingValue<std::string>("issues_repo");
            auto page = dynamic_cast<TextInput*>(__this->getChildByIDRecursive("page"));
            auto sort = dynamic_cast<CCLabelTTF*>(__this->getChildByIDRecursive("sort"));
            auto urlapi = fmt::format(
                "https://api.github.com/repos/{}/issues?per_page={}&page={}&sort={}&",
                issues_repo, per_page, page->getString().data(), sort->getString()
            );
            auto downloadListTaskListener = new EventListener<web::WebTask>;
            auto req = github::web_request();
            downloadListTaskListener->bind(
                [setupListStep3](web::WebTask::Event* e) {
                    if (web::WebResponse* res = e->getValue()) setupListStep3(res);
                }
            );
            downloadListTaskListener->setFilter(req.send("GET", urlapi));
        };

    auto setupList = [__this, scroll_size, paddingx, &scroll, setupListStep2]()
        {
            scroll = ScrollLayer::create(scroll_size);
            scroll->setID("scroll");
            scroll->m_contentLayer->setLayout(
                ColumnLayout::create()
                ->setGap(0.f)
                ->setAutoScale(false)
                ->setAxisReverse(true)
                ->setAxisAlignment(AxisAlignment::End)
            );
            scroll->setPositionX(paddingx / 2 + 2);
            __this->addChild(scroll);
            setupListStep2();
        };

    setupList();

    auto menu = CCMenu::create();
    __this->addChild(menu);

    if (auto Ryzen_profileButton_001 = CCSprite::create("Ryzen_profileButton_001.png"_spr)) {
        Ryzen_profileButton_001->setScale(0.9f);
        auto profileButton = CCMenuItemExt::createSpriteExtra(Ryzen_profileButton_001, 
            [](auto) {
                auto scene = CCScene::create();
                scene->addChild(github::account::user->m_id > 0 ? RznUserLayer(github::account::user) : RznAuthLayer());
                CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
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

    if (auto Ryzen_ReloadBtn_001 = CCSprite::create("Ryzen_ReloadBtn_001.png"_spr)) {
        Ryzen_ReloadBtn_001->setScale(0.9f);
        auto ReloadBtn = CCMenuItemExt::createSpriteExtra(Ryzen_ReloadBtn_001,
            [__this](auto) {
            }
        );
        ReloadBtn->setID("ReloadBtn");
        ReloadBtn->setLayoutOptions(
            AnchorLayoutOptions::create()
            ->setAnchor(Anchor::BottomRight)
            ->setOffset({ -33.f, 33.f })
        );
        menu->addChild(ReloadBtn);
    }

    menu->setLayout(AnchorLayout::create());

    return __this;
}

#include <Geode/modify/CCLayer.hpp>
class ModsLayer : public CCNode {};
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