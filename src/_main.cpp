#include <_main.hpp>
#include <_ini.hpp>
#include <GeodeUI.hpp>
#include <Geode/utils/web.hpp>

class ShittyDragThing : public ScrollLayer {
public:
    void drugs(float) {
        if (!this) return;
        auto pos = this->m_contentLayer->getPosition();
        auto parent = this->getParent();
        if (pos != CCPointZero) {
            parent->setPosition(
                parent->getPositionX() + pos.x,
                parent->getPositionY() + pos.y
            );
        };
        this->m_contentLayer->setPosition(CCPointZero);
    }
    static auto createScroll(CCNode* tar, CCSize size) {
        auto scroll = ScrollLayer::create(size, 0, 0);
        tar->addChild(scroll);
        scroll->m_disableHorizontal = 0;
        scroll->m_disableVertical = 0;
        scroll->setPosition(scroll->getContentSize() / -2);
        scroll->schedule(schedule_selector(ShittyDragThing::drugs));
        return scroll;
    }
};

class MDImagesLoader : public MDTextArea {
public:
    void aminateLoadingLabels() {
        findFirstChildRecursive<CCLabelBMFont>(
            this, [](CCLabelBMFont* label) {
                if (string::containsAll(label->getString(), { "Loading ", "..." })) {
                    label->runAction(CCRepeatForever::create(CCSequence::create(
                        CCEaseSineInOut::create(CCFadeTo::create(0.5f, 90)),
                        CCEaseSineInOut::create(CCFadeTo::create(0.5f, 160)),
                        nullptr
                    )));
                }
                return false;
            }
        );
    }
    void loadit(std::string link) {
        //return void();
        link = string::replace(link, "frame:", "");//bruh
        link = string::replace(link, ":scale_factor=2.0", "");//BRUH
        //log::debug("{}->{}({})", this, __FUNCTION__, link);

        auto filep = dirs::getTempDir() / ("." + std::string(ZipUtils::base64URLEncode(link).c_str()));
        auto req = web::WebRequest();
        auto listener = new EventListener<web::WebTask>;
        listener->bind(
            [this, filep, link](web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {
                    if (res->code() < 399) {
                        res->into(filep);
                        CCTextureCache::get()->removeTextureForKey(filep.string().c_str());
                        CCSpriteFrameCache::get()->removeSpriteFrameByName(link.c_str());
                        CCSpriteFrameCache::get()->addSpriteFrame(
                            CCSprite::create((filep.string() + ":scale_factor=2.0").c_str())->displayFrame(),
                            link.c_str()
                        );
                        if (this and this->isRunning()) this->setString(this->getString());
                        this->aminateLoadingLabels();
                        fs::remove(filep, *(new std::error_code()));
                    }
                }
            }
        );
        listener->setFilter(req.send("GET", link));

    }
    static auto attach(MDTextArea* tar) {
        //set links as frames
        {
            tar->setString(std::regex_replace(
                tar->getString(),
                std::regex("!\\[(.*?)\\]\\((.*?)\\)"), "![Loading $1...](frame:$2:scale_factor=2.0)"
            ).c_str());
        };
        //download links
        {
            auto str = std::string(tar->getString());
            std::regex re("!\\[(.*?)\\]\\((.*?)\\)");
            std::sregex_token_iterator it(str.begin(), str.end(), re, 2);
            std::sregex_token_iterator end;
            for (; it != end; ++it) {
                auto link = it->str();
                reinterpret_cast<MDImagesLoader*>(tar)->loadit(link);
            }
        };
        reinterpret_cast<MDImagesLoader*>(tar)->aminateLoadingLabels();
        return tar;
    }
};

class WebImageNode : public CCNodeRGBA {
public:
    static auto create(std::string url = "", CCSize size = { 30.f, 30.f }, std::function<void(CCSprite*)> loaded = [](auto) {}) {
        auto __this = new WebImageNode();
        __this->init();
        __this->setCascadeColorEnabled(1);
        __this->setCascadeOpacityEnabled(1);
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
                        CCTextureCache::get()->removeTextureForKey(filep.string().c_str());
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
        std::string m_name = "";
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
                "GET", m_url
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
            return (get_token().size() > 12);
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

class RznPost : public CCNode {
public:
    std::string m_name = "unnamed post";
    std::string m_developer = "unnamed post";
    std::string m_desc = "no description";
    std::string m_icon = "";
    std::string m_download = "";
    std::string m_body = "";
    github::user* m_user;
    matjson::Value m_issue;
    CSimpleIni* m_ini;

    static auto create(matjson::Value issue = {}) {
        auto rtn = new RznPost();
        rtn->init();
        rtn->update(issue);
        return rtn;
    }

    void update(matjson::Value issue) {

        m_issue = issue;
        m_user = github::user::create(issue["user"]);

        auto issue_body = m_issue["body"].asString().unwrapOrDefault();

        m_ini = new CSimpleIni;
        m_ini->LoadData(issue_body);

        m_name = m_issue["title"].asString().unwrapOr(m_name);
        m_developer = m_ini->GetValue("main", "developer", m_user->m_login.c_str());
        m_desc = m_ini->GetValue("main", "desc", fmt::format("The {} by {}", m_name, m_developer).c_str());
        m_icon = m_ini->GetValue("main", "icon", m_user->m_avatar_url.c_str());
        m_download = m_ini->GetValue("main", "download", m_download.c_str());

        m_body = fmt::format("# {}\n{}", m_name, m_desc);
        if (issue_body.find("body:") != std::string::npos) {
            //pointas
            auto start = issue_body.find("body:") + 7;
            auto end = issue_body.find("^^^");
            if (end == std::string::npos) end = issue_body.size();
            //a
            std::string substring = issue_body.substr(start, end - start);
            m_body = substring;
        }
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
            "GET", m_issue["url"].asString().unwrapOrDefault()
        ));
    }

    void download() {
        //endpoint
        auto endpoint = m_download;
        auto filename = fs::path(endpoint).filename().string();
        auto path = dirs::getModsDir() / filename;
        auto root = dirs::getGeodeDir();
        ;;;;;;;;;;; path = (string::contains(filename, ".geode")) ? root / "mods" / filename : path;
        path = (string::containsAny(filename, { ".dll", ".so" })) ? root / "libs" / filename : path;
        ;;;;;; path = (string::containsAny(filename, { ".zip" })) ? root / "config" / "geode.texture-loader" / "packs" / filename : path;
        //Popup
        auto pop = createQuickPopup(
            "Download File?",
            "\n \n \n \n \n \n \n",
            "Abort", "Start",
            400.f,
            [this, endpoint, path](auto, bool btn2) {
                if (not this) return;
                if (not btn2) return;
                //updateCustomDownloads();
                //loadingBar
                Ref<Slider> loadingBar = nullptr;
                {
                    loadingBar = Slider::create(this, nullptr);
                    loadingBar->getThumb()->setVisible(0);
                    loadingBar->setID("loadingBar");
                    loadingBar->setPosition(CCPointMake(118.f, 14.f));
                    loadingBar->setZOrder(999);
                    SceneManager::get()->keepAcrossScenes(loadingBar); //this->addChild(loadingBar);
                    //labels
                    auto loadingBarTitle = CCLabelBMFont::create("Loading...", "goldFont.fnt");
                    loadingBarTitle->setAlignment(kCCTextAlignmentCenter);
                    loadingBarTitle->limitLabelWidth(200.f, 0.5f, 0.3f);
                    loadingBarTitle->setID("loadingBarTitle");
                    loadingBarTitle->setPositionY(24.f);
                    loadingBar->addChild(loadingBarTitle);
                    //labels
                    auto closeBtn = CCMenu::createWithItem(CCMenuItemExt::createSpriteExtraWithFrameName(
                        "edit_delBtnSmall_001.png", 0.8f, [loadingBar](auto) {
                            createQuickPopup("Cancel Download",
                                "Are you sure you want to cancel the downloading?",
                                "NO", "Yes",
                                [loadingBar](CCNode*, bool ok) {
                                    if (!ok) return;
                                    SceneManager::get()->forget(loadingBar);
                                    loadingBar->removeFromParent();
                                }
                            );
                        }
                    ));
                    closeBtn->setID("loadingBarCloseBtn");
                    closeBtn->setPosition(CCPointMake(106.f, 32.f));
                    loadingBar->addChild(closeBtn);
                    //drag
                    auto dragTouchZone = ShittyDragThing::createScroll(loadingBar, { 212.f, 48.f });
                    dragTouchZone->setPositionY(-10.f);
                    //bg
                    auto bg = CCScale9Sprite::create("square02_small.png");
                    bg->setOpacity(120);
                    bg->setAnchorPoint({ 0.5f, 0.2f });
                    bg->setContentSize({ 226.f, 48.f });
                    loadingBar->addChild(bg, -1);
                }
                //req
                auto req = web::WebRequest();
                auto listener = new EventListener<web::WebTask>;
                listener->bind(
                    [this, endpoint, path, loadingBar](web::WebTask::Event* e) {
                        auto l_end = [this, path](web::WebResponse* res)
                            {
                                res->into(path);
                                if (string::contains(path.string(), "geode.texture-loader")) {
                                    FLAlertLayer* asd = geode::createQuickPopup(
                                        "Pack Downloaded",
                                        "You can go back to packs select menu and apply it...",
                                        "OK", nullptr,
                                        [](auto, auto) {
                                            //if (PackSelectLayer::lastCreatedOne) PackSelectLayer::lastCreatedOne->reloadList();
                                        }
                                    );
                                }
                                else {
                                    FLAlertLayer* asd = geode::createQuickPopup(
                                        "Restart Game?",
                                        "To load new mod",
                                        "Later", "Yes",
                                        [](auto, bool btn2) {
                                            if (btn2) utils::game::restart();
                                        }
                                    );
                                }
                            };
                        auto l_prog = [this, loadingBar, e](web::WebProgress* prog) {
                            if (!loadingBar or !loadingBar->isRunning()) return e->cancel();
                            double d1_of100 = prog->downloadProgress().value_or(0.0);
                            double d1 = prog->downloaded();
                            double d2 = prog->downloadTotal();
                            /*log::debug(
                                "\n"
                                "d1_of100:{}\n"
                                "      d1:{}\n"
                                "      d2:{}",
                                d1_of100, d1, d2
                            );*/
                            //return;
                            if (d1 <= 0) return;
                            if (d2 <= 0) return;
                            if (this == nullptr) return;
                            //vars
                            auto strProgOneOf = fmt::format("{} of {}", string::size(d1), string::size(d2));
                            auto strName = fmt::format("{}", this->m_name);
                            //loadingBar
                            if (loadingBar) {
                                if (d1 == d2) {
                                    SceneManager::get()->forget(loadingBar);
                                    loadingBar->setVisible(0);
                                    loadingBar->removeFromParent();
                                }
                                else {
                                    if (loadingBar) loadingBar->setValue(d1_of100 / 100);
                                    if (auto loadingBarTitle = typeinfo_cast<CCLabelBMFont*>(loadingBar->getChildByIDRecursive("loadingBarTitle")))
                                        loadingBarTitle->setString((strName + "\n" + strProgOneOf).data());
                                };
                            }
                            };
                        auto l_error = [this, endpoint](std::string const& error) {
                            auto asd = geode::createQuickPopup(
                                "Request exception",
                                error + "\n" + endpoint,
                                "Nah", nullptr, 420.f, nullptr, false
                            );
                            asd->m_scene = this;
                            asd->show();
                            };
                        if (web::WebProgress* prog = e->getProgress()) {
                            l_prog(prog);
                        }
                        if (web::WebResponse* res = e->getValue()) {
                            std::string data = res->string().unwrapOr("no res");
                            //call the some shit
                            if (res->code() < 399) l_end(res);
                            else l_error(data);
                        }
                    }
                );
                listener->setFilter(req.send("GET", endpoint));
            }
        );
        if (pop) {
            auto body_data = fmt::format(
                "# From:" "\n"
                "{}" "\n"
                "# To:" "\n"
                "<cj>__{}__",
                endpoint,
                path.string()
            );
            auto mdtextarea = MDTextArea::create(body_data, { 360.f, 140.f });
            mdtextarea->setPositionY(96.f);//(-(mdtextarea->getContentSize() / 2));
            pop->m_buttonMenu->addChild(mdtextarea);
            handleTouchPriority(pop);
            if (auto delte_btnspr = ButtonSprite::create("   Uninstall   ", "goldFont.fnt", "GJ_button_05.png")) {
                delte_btnspr->setScale(0.7f);
                auto delete_item = CCMenuItemSpriteExtra::create(delte_btnspr, this, nullptr);
                delete_item->setID("delete");
                delete_item->setAnchorPoint({ 0.5f, 1.f });
                delete_item->setPositionY(-33.f);
                if (fs::exists(path)) pop->m_buttonMenu->addChild(delete_item);
            }
        };
    }

};

inline CCNode* RznPostItem(RznPost* post, float width);
inline CCNode* RznCommentItem(matjson::Value json, float width) {
    return nullptr;//to-do yooo
};
inline CCNode* RznReactionsRow(matjson::Value json, float width) {
    //react_row
    if (json.contains("reactions")) {
        auto react_row = CCMenu::create();
        auto& reactions = json["reactions"];
        auto onBtn = [reactions](CCMenuItemSpriteExtra* sender)
            {
                auto tab = typeinfo_cast<GeodeTabSprite*>(sender->getNormalImage());
                auto should_delete = tab->m_selectedBG->isVisible();
                sender->setEnabled(0);
                auto req = github::web_request();
                auto listener = new EventListener<web::WebTask>;
                listener->bind([tab, sender, should_delete](web::WebTask::Event* e)
                    {
                        if (web::WebResponse* res = e->getValue()) {
                            sender->setEnabled(1);
                            auto string = res->string();
                            auto json = res->json().unwrapOr(matjson::Value());
                            if (res->code() < 399) {
                                tab->select(!should_delete);
                                sender->setTag(json["id"].asInt().unwrapOr(sender->getTag()));
                                auto count = numFromString<int>(tab->m_label->getString()).unwrapOr(1);
                                if (should_delete) --count; else ++count;
                                tab->m_label->setString(fmt::format("{}", count).data());
                            }
                        }
                        else if (e->isCancelled()) {
                            sender->setEnabled(1);
                        }
                    }
                );
                auto url = reactions["url"].asString().unwrapOr("");
                if (should_delete) {
                    listener->setFilter(req.send(
                        "DELETE", fmt::format("{}/{}", url, sender->getTag())
                    ));
                }
                else {
                    auto body = matjson::Value();
                    body["content"] = sender->getID();
                    req.bodyJSON(body);
                    listener->setFilter(req.send(
                        "POST", url
                    ));
                };
            };
        auto addBtn = [reactions, react_row, onBtn](std::string name, std::string frame, std::string text = "")
            {
                auto btnspr = GeodeTabSprite::create(
                    frame.data(),
                    (not text.empty() ?
                        text : string::replace(
                            reactions[name].dump(), "\"", "")
                        ).data(),
                    52.f, 1
                );
                btnspr->select(0);
                btnspr->m_deselectedBG->setContentHeight(32.f);
                btnspr->m_selectedBG->setContentHeight(32.f);
                btnspr->m_label->setFntFile("gjFont17.fnt");
                btnspr->m_label->setScale(0.2f + btnspr->m_label->getScale());
                btnspr->setScale(0.6f);
                auto btn = CCMenuItemExt::createSpriteExtra(btnspr, onBtn);
                btn->setID(name.data());
                react_row->addChild(btn);
                return btn;
            };
        auto plus1 = addBtn("+1", "+1.png"_spr);
        auto minus1 = addBtn("-1", "-1.png"_spr);
        auto laugh = addBtn("laugh", "laugh.png"_spr);
        auto confused = addBtn("confused", "confused.png"_spr);
        auto heart = addBtn("heart", "heart.png"_spr);
        auto hooray = addBtn("hooray", "hooray.png"_spr);
        auto rocket = addBtn("rocket", "rocket.png"_spr);
        auto eyes = addBtn("eyes", "eyes.png"_spr);
        auto loading = LoadingSpinner::create(1.f);
        react_row->addChild(loading, 0, 1);
        react_row->setTouchEnabled(0);
        react_row->setContentHeight(18.000f);//temp
        react_row->setContentWidth(width);
        react_row->setLayout(
            RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setAutoScale(0)
            ->setGrowCrossAxis(1)
            ->setCrossAxisOverflow(0)
        );
        react_row->setContentHeight(20.000f);
        //loadthereactionsfk
        std::function<void()> load;
        auto listener = new EventListener<web::WebTask>;
        auto bindfn = [reactions, load, react_row, loading](web::WebTask::Event* e)
            {
                if (web::WebResponse* res = e->getValue()) {
                    auto json = res->json();
                    auto string = res->string();
                    if (res->code() < 399) if (json.isOk()) {
                        auto val = json.unwrapOrDefault();
                        if (val.asArray().isOk()) for (auto reaction : val.asArray().unwrap()) {
                            auto itsme = reaction["user"]["id"].asInt().unwrapOrDefault() == github::account::user->m_id;
                            auto name = reaction["content"].asString().unwrapOrDefault();
                            auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(react_row->getChildByID(name));
                            auto tab = typeinfo_cast<GeodeTabSprite*>(item->getNormalImage());
                            if (itsme) tab->select(itsme);
                            item->setTag(reaction["id"].asInt().unwrapOrDefault());//for delete
                        };
                        auto page = loading ? loading->getTag() : 1;
                        loading->setTag(page + 1);
                        if (json.unwrapOrDefault().asArray().unwrap().size() < 100) {
                            //final
                            loading->removeFromParent();
                            react_row->setTouchEnabled(1);
                        }
                        else load();
                    }
                };
            };
        load = [listener, bindfn, reactions, loading]()
            {
                auto req = github::web_request();
                if (loading) req.param("page", loading->getTag());
                req.param("per_page", 100);
                listener->bind(bindfn);
                listener->setFilter(req.get(
                    reactions["url"].asString().unwrapOr("")
                ));
            };
        load();
        return react_row;
    }
    return CCNode::create();
};

class RznLayer : public CCLayer {
public:
    static auto create(std::function<void()> onBack) {

        auto rtn = new RznLayer;
        rtn->init();

        if (SETTING(bool, "rzn_menu_music")) GameManager::sharedState()->fadeInMusic(
            "Fishbone.mp3" ""_spr
        );
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

inline RznLayer* RznAuthLayer() {
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
        text1 << "## [" << (user->m_name.empty() ? user->m_login : user->m_name) << "]("<< user->m_html_url << ")" << std::endl;
        text1 << "<c-999999>" << user->m_login << ", id: " << user->m_id << "</c>" << std::endl << std::endl;
        if (user->m_bio.size() > 1) text1 << "`" << user->m_bio << "`" << std::endl << std::endl;
        if (user->m_json["followers"].isNumber()) text1 << dot << "<c-AAAAAA>Followers: </c>" << user->m_json["followers"].asInt().unwrapOrDefault();
        if (user->m_json["following"].isNumber()) text1 << ", <c-AAAAAA>Following: </c>" << user->m_json["following"].asInt().unwrapOrDefault() << std::endl << std::endl;
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
            fmt::format("```\n{}\n```", user->m_json.dump()), { 280, 260 }
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

inline void OpenRznUserLayer(github::user* user) {

    auto layer = RznUserLayer(user);

    auto scene = CCScene::create();
    scene->addChild(layer);
    CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));

    user->reload(
        [layer, user] {
            if (layer) if (auto scene = layer->getParent()) scene->addChild(RznUserLayer(user));
            if (layer) layer->removeFromParent();
        }
    );

}

inline RznLayer* RznPostLayer(RznPost* post) {
    auto __this = RznLayer::create(
        []() {
            CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
        }
    );

    auto its_me = post->m_user->m_id == github::account::user->m_id;

    if (auto center_menu = CCMenu::create()) {

        auto size = CCSizeMake(450.f, 350.f);

        center_menu->setContentWidth(size.width);
        __this->addChild(center_menu);

        center_menu->addChild(RznPostItem(post, size.width));

        if (auto middle_line = CCMenu::create()) {
            middle_line->setID("middle_line");
            center_menu->addChild(middle_line);
            middle_line->setContentWidth(center_menu->getContentWidth());

            auto reactions = RznReactionsRow(post->m_issue, 292.f);
            reactions->setAnchorPoint({ 0,0.5f });
            middle_line->addChild(reactions);

            if (auto downloadBtn = ButtonSprite::create("Download", "goldFont.fnt", "GJ_button_05.png")) {
                if (fs::exists("download_path")) {
                    downloadBtn->m_label->setString("Update");
                    downloadBtn->m_label->setScale(
                        (downloadBtn->m_BGSprite->getContentWidth() - 15) / downloadBtn->m_label->getContentWidth()
                    );
                }
                if (downloadBtn->m_label->getScale() > 0.9f) downloadBtn->m_label->setScale(0.9f);
                downloadBtn->setScale(0.8f);
                downloadBtn->setID("downloadBtn");
                auto item = CCMenuItemExt::createSpriteExtra(downloadBtn, 
                    [post](auto){
                        post->download();
                    }
                );
                item->setID("download");
                middle_line->addChild(item);
            }

            middle_line->setLayout(RowLayout::create()
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::Between)
                ->setCrossAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisLineAlignment(AxisAlignment::Start)
            );
        }


        auto body = MDImagesLoader::attach(MDTextArea::create(post->m_body, { size.width + 10, 260 }));
        center_menu->addChild(body);

        center_menu->setContentSize(size);
        center_menu->setLayout(ColumnLayout::create()
            ->setAxisReverse(1)
        );

        auto limitation = __this->getContentSize();
        limitation.height -= 12.f;
        limitation.width -= 160.f;
        limitNodeSize(center_menu, limitation, 7.77f, 0.3f);

        //lol
        if (auto aw = center_menu->getChildByIDRecursive("middle_line")) {
            aw->setAnchorPoint(CCPointMake(0.5f, 0.f));
            auto line = CCSprite::createWithSpriteFrameName("floorLine_01_001.png");
            line->setAnchorPoint(CCPointMake(0.f, 6.5f));
            line->setScaleX(aw->getContentWidth() / line->getContentWidth());
            aw->addChild(line);
        }

    };

    if (auto menu = CCMenu::create()) {

        __this->addChild(menu);

        if (auto Ryzen_ReloadBtn_001 = CCSprite::create("Ryzen_ReloadBtn_001.png"_spr)) {
            Ryzen_ReloadBtn_001->setScale(0.9f);
            auto ReloadBtn = CCMenuItemExt::createSpriteExtra(Ryzen_ReloadBtn_001,
                [__this, post](auto) {
                    post->reload(
                        [__this, post] {
                            if (__this) if (auto scene = __this->getParent()) scene->addChild(RznPostLayer(post));
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

inline void OpenRznPostLayer(RznPost* post) {

    auto layer = RznPostLayer(post);

    auto scene = CCScene::create();
    scene->addChild(layer);
    CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));

    post->reload(
        [layer, post] {
            if (layer) if (auto scene = layer->getParent()) scene->addChild(RznPostLayer(post));
            if (layer) layer->removeFromParent();
        }
    );

}

inline RznLayer* RznListLayer() {
    auto __this = RznLayer::create(
        []() {
            CCDirector::get()->popSceneWithTransition(0.5f, kPopTransitionFade);
            GameManager::get()->fadeInMenuMusic();
        }
    );

    CCMenuItem* profile = CCMenuItem::create();
    CCMenuItem* reload = CCMenuItem::create();

    std::function<void()> setupList = [](){};
    std::function<void()> setupListLoad = []() {};
    if ("shitcode") {

        auto paddingx = 160.f;
        auto paddingt = 0.f;
        CCSize scroll_size = { CCDirector::get()->getScreenRight() - paddingx - 4, CCDirector::get()->getScreenTop() - paddingt };

        ScrollLayer* scroll;

        auto setupListStep4 = [__this, scroll, scroll_size](matjson::Value issues)
            {
                log::debug("posts: {}", issues.dump());
                auto scroll = typeinfo_cast<ScrollLayer*>(__this->getChildByID("scroll"));
                if (not scroll) return;

                scroll->m_contentLayer->removeAllChildrenWithCleanup(0);

                for (auto issue : issues) {
                    auto post = RznPost::create(issue);
                    auto item = RznPostItem(post, scroll->m_contentLayer->getContentWidth());
                    scroll->m_contentLayer->addChild(item);
                }

                scroll->m_contentLayer->setLayout(RowLayout::create()
                    ->setGrowCrossAxis(1)
                    ->setCrossAxisAlignment(AxisAlignment::End)
                    ->setAxisAlignment(AxisAlignment::Start)
                );

                auto bg = CCLayerColor::create({0,0,0,90});
                bg->setContentSize(scroll->getContentSize());
                bg->setPosition(scroll->getPosition());
                bg->setScale(1.05f);
                bg->setID("scroll-bg");
                scroll->getParent()->removeChildByID("scroll-bg");
                scroll->getParent()->addChild(bg, -1);

                auto scrlbar = Scrollbar::create(scroll);
                scrlbar->setPosition(scroll->getPosition());
                scrlbar->setPositionX(scroll->getPositionX() + scroll->getContentWidth());
                scrlbar->setAnchorPoint({ -0.250f, 0.f });
                scrlbar->getChildByType(0)->setScaleY(5.f);
                scrlbar->getChildByType(1)->setScaleY(0.475f);
                scrlbar->setID("scroll-scrlbar");
                scroll->getParent()->removeChildByID("scroll-scrlbar");
                scroll->getParent()->addChild(scrlbar);

                auto leftb = Scrollbar::create(scroll);
                leftb->setPosition(scroll->getPosition());
                leftb->setAnchorPoint({ 1.250f, 0.f });
                leftb->getChildByType(0)->setScaleY(5.f);
                leftb->getChildByType(1)->setVisible(0);
                leftb->setID("scroll-leftb");
                scroll->getParent()->removeChildByID("scroll-leftb");
                scroll->getParent()->addChild(leftb);

                //fix
                if (scroll->m_contentLayer->getContentHeight() > scroll->getContentHeight()) {
                }
                else {
                    scroll->m_disableMovement = 1;
                }

            };

        auto setupListLoadFinish = [__this, setupListStep4](web::WebResponse* res)
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

        setupListLoad = [__this, scroll, setupListLoadFinish]()
            {
                auto per_page = Mod::get()->getSettingValue<int64_t>("per_page");
                auto issues_repo = Mod::get()->getSettingValue<std::string>("issues_repo");
                //auto page = typeinfo_cast<TextInput*>(__this->getChildByIDRecursive("page"));
                //auto sort = typeinfo_cast<CCLabelTTF*>(__this->getChildByIDRecursive("sort"));
                auto urlapi = fmt::format(
                    "https://api.github.com/repos/{}/issues?per_page={}&page={}&sort={}&",
                    issues_repo, per_page, "1", "created"//page->getString().data(), sort->getString()
                );
                auto downloadListTaskListener = new EventListener<web::WebTask>;
                auto req = github::web_request();
                downloadListTaskListener->bind(
                    [setupListLoadFinish](web::WebTask::Event* e) {
                        if (web::WebResponse* res = e->getValue()) setupListLoadFinish(res);
                    }
                );
                downloadListTaskListener->setFilter(req.send("GET", urlapi));
            };

        setupList = [__this, scroll_size, paddingx, &scroll, setupListLoad]()
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
                setupListLoad();
            };

    };
    setupList();

    if (auto menu = CCMenu::create()) {
        __this->addChild(menu);

        if (auto Ryzen_profileButton_001 = CCSprite::create("Ryzen_profileButton_001.png"_spr)) {
            Ryzen_profileButton_001->setScale(0.9f);
            auto profile = CCMenuItemExt::createSpriteExtra(Ryzen_profileButton_001,
                [](auto) {
                    auto scene = CCScene::create();
                    scene->addChild(github::account::user->m_id > 0 ? RznUserLayer(github::account::user) : RznAuthLayer());
                    CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
                }
            );
            profile->setID("profile");
            profile->setLayoutOptions(
                AnchorLayoutOptions::create()
                ->setAnchor(Anchor::BottomLeft)
                ->setOffset({ 33.f, 33.f })
            );
            menu->addChild(profile);
        }

        if (auto Ryzen_ReloadBtn_001 = CCSprite::create("Ryzen_ReloadBtn_001.png"_spr)) {
            Ryzen_ReloadBtn_001->setScale(0.9f);
            reload = CCMenuItemExt::createSpriteExtra(Ryzen_ReloadBtn_001,
                [setupListLoad](auto) {
                    setupListLoad();
                }
            );
            reload->setID("reload");
            reload->setLayoutOptions(
                AnchorLayoutOptions::create()
                ->setAnchor(Anchor::BottomRight)
                ->setOffset({ -33.f, 33.f })
            );
            menu->addChild(reload);
        }

        menu->setLayout(AnchorLayout::create());
    }

    return __this;
}

inline CCNode* RznPostItem(RznPost* post, float width) {

    auto item = CCNode::create();
    item->setID("item");
    item->setContentWidth(width);

    auto container = CCNode::create();
    container->setID("container");
    container->setContentWidth(item->getContentWidth());

    if (auto main_row = CCNode::create()) {
        main_row->setID("main_row");

        container->addChild(main_row);
        main_row->setContentWidth(container->getContentWidth());

        main_row->setLayout(RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisLineAlignment(AxisAlignment::End)
        );


        auto image = WebImageNode::create(post->m_icon, { 64, 64 });
        image->setID("image");

        auto image_item = CCMenuItemExt::createSpriteExtra(
            image, [post](auto) {
                OpenRznPostLayer(post);
            }
        );
        image_item->m_animationEnabled = 0;
        image_item->m_colorEnabled = 1;
        image_item->setAnchorPoint(CCPointMake(0.f, 0.f));
        image_item->setID("image_item");

        main_row->addChild(CCMenu::createWithItem(image_item));

        image_item->getParent()->setID("image_item_menu");
        image_item->getParent()->setContentSize(image_item->getContentSize());
        image_item->getParent()->setPosition(image_item->getPosition());
        image_item->getParent()->setAnchorPoint(CCPointMake(0.f, 1.0f));

        main_row->updateLayout();

        if (auto left_row = CCNode::create()) {
            left_row->setID("left_row");

            main_row->addChild(left_row);
            left_row->setContentWidth(
                main_row->getContentWidth()
                -
                main_row->getChildByType(-1)->getPosition().getDistance(main_row->getChildByType(-1)->getPosition())
            );
            left_row->setContentHeight(main_row->getContentHeight());

            if (auto line1 = CCNode::create()) {
                line1->setAnchorPoint(CCPointMake(0.f, 0.9f));
                line1->setContentWidth(left_row->getContentWidth());
                line1->setID("line1");

                auto name = CCLabelBMFont::create(
                    post->m_name.c_str(),
                    "geode.loader/mdFontB.fnt"
                );
                name->setID("name");
                limitNodeHeight(name, 19.f, 0.9f, 0.1f);

                auto name2 = CCLabelBMFont::create(name->getString(), name->getFntFile());
                name2->setID("name2");
                name2->setOpacity(190);
                limitNodeHeight(name2, 19.f, 0.9f, 0.1f);

                auto name_item = CCMenuItemExt::createSprite(
                    name, name2, [post](auto) {
                        OpenRznPostLayer(post);
                    }
                );
                name_item->setID("name_item");
                name_item->setAnchorPoint(CCPointMake(0.f, 0.f));
                name_item->setContentSize(name->getScaledContentSize());

                line1->addChild(CCMenu::createWithItem(name_item));

                name_item->getParent()->setID("name_item_menu");
                name_item->getParent()->setContentSize(name_item->getContentSize());
                name_item->getParent()->setPosition(name_item->getPosition());
                name_item->getParent()->setAnchorPoint(CCPointMake(0.f, 0.0f));

                auto user = CCLabelBMFont::create(
                    ("by " + post->m_developer).c_str(),
                    "chatFont.fnt"
                );
                user->setID("user");
                user->setOpacity(180);

                auto user2 = CCLabelBMFont::create(user->getString(), user->getFntFile());
                user2->setID("user2");
                user2->setOpacity(90);

                auto user_item = CCMenuItemExt::createSprite(
                    user, user2, [post](auto) {
                        OpenRznUserLayer(post->m_user);
                    }
                );
                user_item->setID("user_item");
                user_item->setAnchorPoint(CCPointMake(0.f, 0.f));

                line1->addChild(CCMenu::createWithItem(user_item));

                user_item->getParent()->setID("user_item_menu");
                user_item->getParent()->setContentSize(user_item->getContentSize());
                user_item->getParent()->setPosition(user_item->getPosition());
                user_item->getParent()->setAnchorPoint(CCPointMake(1.f, 0.5f));

                line1->setLayout(RowLayout::create()
                    ->setAxisAlignment(AxisAlignment::Start)
                    ->setCrossAxisLineAlignment(AxisAlignment::Start)
                );

                left_row->addChild(line1);
            }

            if (auto line2 = CCNode::create()) {
                line2->setAnchorPoint(CCPointMake(0.f, 0.9f));
                line2->setContentWidth(left_row->getContentWidth());
                line2->setID("line2");

                auto desc = CCLabelBMFont::create(
                    post->m_desc.c_str(), "chatFont.fnt"
                );
                desc->setID("desc");
                limitNodeWidth(desc, line2->getContentWidth(), 0.9f, 0.1f);


                line2->addChild(desc);

                line2->setLayout(RowLayout::create()
                    ->setAxisAlignment(AxisAlignment::Start)
                    ->setCrossAxisLineAlignment(AxisAlignment::Start)
                );

                left_row->addChild(line2);
            }

            if (auto line3 = CCMenu::create()) {
                line3->setAnchorPoint(CCPointMake(0.f, 0.9f));
                line3->setContentWidth(left_row->getContentWidth());
                line3->setContentHeight(0.f);
                line3->setID("line3");

                //tags
                for (auto catgirl : post->m_issue["labels"]) {
                    auto color = cocos::cc3bFromHexString(catgirl["color"].asString().unwrapOrDefault()).unwrapOr(ccColor3B(190, 190, 200));
                    //label
                    auto label = CCLabelBMFont::create(catgirl["name"].asString().unwrapOrDefault().c_str(), "geode.loader/mdFontB.fnt");
                    auto hsv = cchsv(0, 0.1, 0.30, 1, 1);
                    label->setColor(GameToolbox::transformColor(color, hsv));
                    label->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
                    label->setScale(0.5f);
                    auto label_sel = CCLabelBMFont::create(label->getString(), label->getFntFile());
                    label_sel->setColor(color);
                    label_sel->setScale(0.5f);
                    //item
                    auto item = CCMenuItemExt::createSprite(
                        label, label_sel, [catgirl, label](CCMenuItem* item){
                            //node
                            if (!item) return;
                            //pop
                            auto pop = geode::MDPopup::create(
                                catgirl["name"].asString().unwrapOrDefault().c_str(),
                                "<c-" + catgirl["color"].asString().unwrapOrDefault()+ ">" + catgirl["description"].asString().unwrapOr("<co>no description...</c>\n<cy>seems to be created label by publisher via !setlabels comment"),
                                "OK"
                            );
                            public_cast(pop, m_closeBtn)->setVisible(0);
                            if (auto title = pop->m_mainLayer->getChildByType<CCLabelBMFont>(-1)) {
                                title->setFntFile(label->getFntFile());
                                title->setColor(label->getColor());
                                title->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
                            };
                            pop->show();
                        }
                    );
                    item->setContentSize(label->getScaledContentSize());
                    item->setID(catgirl["id"].dump());
                    line3->addChild(item);
                }

                line3->setLayout(RowLayout::create()
                    ->setAxisAlignment(AxisAlignment::Start)
                    ->setCrossAxisLineAlignment(AxisAlignment::Start)
                );

                left_row->addChild(line3);
            }

            left_row->setAnchorPoint(CCPointMake(0.f, 1.f));
            left_row->setLayout(ColumnLayout::create()
                ->setGrowCrossAxis(true)
                ->setAxisReverse(true)
                ->setAxisAlignment(AxisAlignment::Even)
                ->setCrossAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisLineAlignment(AxisAlignment::Start)
            );
        }

        main_row->updateLayout();
    };

    item->addChild(container);

    container->setLayout(RowLayout::create());
    item->setContentSize(container->getContentSize() + (CCSize(1, 1) * 12));

    container->setAnchorPoint({ 0.5f,0.5f });
    container->setPosition(item->getContentSize() / 2);

    auto bg = CCScale9Sprite::create("square02_001.png");
    bg->setID("bg");
    bg->setContentSize(item->getContentSize());
    bg->setPosition(item->getContentSize() / 2);
    bg->setOpacity(75);
    item->addChild(bg, -1);

    return item;
}


//your favorite part
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
    $override bool init() {
        if (!CCLayer::init()) return false;
        if (typeinfo_cast<ModsLayer*>(this)) queueInMainThread([this] { customSetup(); });
        return true;
    }
};

#include <Geode/modify/CCSprite.hpp>
class $modify(CCSpriteExt, CCSprite) {
    static void onModify(auto & self) {
        auto names = {
            "cocos2d::CCSprite::create",
            "cocos2d::CCSprite::createWithSpriteFrameName",
        };
        for (auto name : names) if (!self.setHookPriorityPost(name, Priority::Last)) {
            log::error("Failed to set hook priority for {}.", name);
        }
    }
    $override static CCSprite* create(const char* pszFileName) {
        std::string str = pszFileName;
        std::regex re(":scale_factor=([0-9.]+)");
        std::smatch match;
        if (std::regex_search(str, match, re)) {

            auto target_factor = std::stof(match[1].str());
            auto old_factor = CC_CONTENT_SCALE_FACTOR();

            CCDirector::sharedDirector()->setContentScaleFactor(target_factor);
            auto sprite = CCSprite::create(std::regex_replace(str, re, "").c_str());
            CCDirector::sharedDirector()->setContentScaleFactor(old_factor);

            return sprite;
        }
        return CCSprite::create(pszFileName);
    }
    $override static CCSprite* createWithSpriteFrameName(const char* pszSpriteFrameName) {
        std::string str = pszSpriteFrameName;
        std::regex re(":scale_factor=([0-9.]+)");
        std::smatch match;
        if (std::regex_search(str, match, re)) {

            auto target_factor = utils::numFromString<float>(match[1].str()).unwrapOr(1.f);
            auto old_factor = CC_CONTENT_SCALE_FACTOR();

            CCDirector::sharedDirector()->setContentScaleFactor(target_factor);
            auto sprite = CCSprite::createWithSpriteFrameName(std::regex_replace(str, re, "").c_str());
            CCDirector::sharedDirector()->setContentScaleFactor(old_factor);

            return sprite;
        }
        return CCSprite::createWithSpriteFrameName(pszSpriteFrameName);
    }
};