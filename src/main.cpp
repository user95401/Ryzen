#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/Utils.hpp>
#include <Geode/ui/TextInput.hpp>
using namespace geode::prelude;

#include <regex>

#include "SimpleIni.h"

template <typename T>
bool checkExistence(T filename)
{
    std::ifstream Infield(filename);
    return Infield.good();
}
template <typename T>
void remove_dir(T path) {
    ghc::filesystem::remove_all(path);
    //lol
    //system(fmt::format("rd /s /q \"{}\"", path).data());
// !GEODE_IS_WINDOWS
}
auto read_file(ghc::filesystem::path path) -> std::string {
    constexpr auto read_size = std::size_t(4096);
    auto stream = std::ifstream(path);
    stream.exceptions(std::ios_base::badbit);
    if (not stream) {
        return fmt::format("file does not exist ({}({}) error)", __func__, path);
    }
    auto out = std::string();
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}
std::string convertSize(size_t size) {
    static const char* SIZES[] = { "B", "KB", "MB", "GB" };
    int div = 0;
    size_t rem = 0;
    while (size >= 1024 && div < (sizeof SIZES / sizeof * SIZES)) {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }
    //roundOff size_d
    double size_d = (float)size + (float)rem / 1024.0;
    double d = size_d * 100.0;
    int i = d + 0.5;
    d = (float)i / 100.0;
    //result
    std::string result = fmt::to_string(d) + "" + SIZES[div];
    return result;
}
std::string abbreviateNumber(int num) {
    double n = static_cast<double>(num);
    char suffix = 0;
    if (num >= 1000000) {
        n /= 1000000;
        suffix = 'M';
    }
    else if (num >= 1000) {
        n /= 1000;
        suffix = 'K';
    }
    else return fmt::format("{}", num);
    return fmt::format("{:.1f}{}", n, suffix);
}
std::vector<std::string> explode(std::string separator, std::string input) {
    std::vector<std::string> vec;
    for (int i{ 0 }; i < input.length(); ++i) {
        int pos = input.find(separator, i);
        if (pos < 0) { vec.push_back(input.substr(i)); break; }
        int count = pos - i;
        vec.push_back(input.substr(i, count));
        i = pos + separator.length() - 1;
    }
    if (vec.size() == 0) vec.push_back(input);/*
    std::stringstream log;
    for (auto item : vec)
        log << std::endl << item << std::endl;
    log::debug("{}(separator \"{}\", input \"{}\").rtn({})", __FUNCTION__, separator, input, log.str());*/
    return vec;
}
#define public_cast(value, member) [](auto* v) { \
	class FriendClass__; \
	using T = std::remove_pointer<decltype(v)>::type; \
	class FriendeeClass__: public T { \
	protected: \
		friend FriendClass__; \
	}; \
	class FriendClass__ { \
	public: \
		auto& get(FriendeeClass__* v) { return v->member; } \
	} c; \
	return c.get(reinterpret_cast<FriendeeClass__*>(v)); \
}(value)

auto ryzen_dir = dirs::getGeodeDir() / "ryzen";

#define ghapiauth \
.userAgent(Mod::get()->getID()) \
.header("X-GitHub-Api-Version", "2022-11-28") \
.header(AuthorizationHeaderName(), fmt::format("Bearer {}", get_ghc()))
auto ghc_file = ryzen_dir / ".ghc";
const char* AuthorizationHeaderName() {
    return checkExistence(ghc_file) ? "Authorization" : "AuthDisabled";
}
const char* get_ghc() {
    ghc::filesystem::create_directories(ryzen_dir);
    return read_file(ghc_file).data();
}
void set_ghc(std::string token) {
    ghc::filesystem::create_directories(ryzen_dir);
    std::ofstream(ghc_file) << token;
}
class GitHubAuthPopup : public FLAlertLayer, FLAlertLayerProtocol {
public:
    virtual void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
        auto protocol = new GitHubAuthPopup;
        //info"Continue"
        if (p0->getID() == "info" and p1) {
            //open auth apps idk
            web::openLinkInBrowser("https://ryzen.7m.pl/auth");
            //finish pop
            auto pop = FLAlertLayer::create(
                protocol,
                "Authorization",
                "Put code from gray page here:" "\n \n \n",
                "Back", "Finish",
                360.f
            );
            //input
            auto input = InputNode::create(280.f, "the code");
            input->setID("input");
            input->setPositionY(42.f);
            pop->m_buttonMenu->addChild(input);
            //paste
            auto paste = CCMenuItemSpriteExtra::create(
                CCLabelBMFont::create(
                    "paste\ntext",
                    "Comic Sans MS.fnt"_spr
                ),
                pop,
                menu_selector(GitHubAuthPopup::onPasteToInput)
            );
            paste->setPositionY(90.f);
            paste->setPositionX(-140.f);
            pop->m_buttonMenu->addChild(paste);
            //last popup setup
            pop->setID("finish");
            pop->show();
        };
        //finish"Finish"
        if (p0->getID() == "finish" and p1) {
            //code
            auto code = std::string("");
            auto input = dynamic_cast<InputNode*>(p0->getChildByIDRecursive("input"));
            if (input) code = input->getString();
            //
            auto a = [this, protocol](matjson::Value const& catgirl) {
                if (not catgirl.contains("access_token")) {
                    auto asd = geode::createQuickPopup(
                        "Failed getting token",
                        catgirl.dump(),
                        "Nah", nullptr, 420.f, nullptr, false
                    );
                    asd->m_scene = this;
                    asd->show();
                    return;
                }
                set_ghc(catgirl["access_token"].as_string());
                Notification::create("Access token saved")->show();
                };
            auto b = [this](std::string const& error)
                {// something went wrong with our web request Q~Q
                    auto message = error;
                    auto asd = geode::createQuickPopup(
                        "Request exception",
                        message,
                        "Nah", nullptr, 420.f, nullptr, false
                    );
                    asd->m_scene = this;
                    asd->show();
                };
            web::AsyncWebRequest()
                .header("Accept", "application/json")
                .bodyRaw(
                    fmt::format("code={}", code) +
                    "&" "client_id=Ov23li0XhdQqNS3Bf3s4"
                    "&" "client_secret=2480254654448da5c88358c85d632dc6fd31010a"
                    //"&" ""
                )
                .post("https://github.com/login/oauth/access_token")
                .json().then(a).expect(b);
        }
        //back"Back"
        if (p0->getID() == "finish" and not p1) {
            show_info();
        }
    };
    void show_info() {
        auto protocol = new GitHubAuthPopup;
        auto pop = FLAlertLayer::create(
            protocol,
            "Authorization", 
            "<co>Authorize</c> your <cy>GitHub Account</c> to <cg>reduce</c> <cr>limits</c> and be <cg>able to create comments</c> in game.\nAfter click on the <co>\"Continue\" button</c> you will be <cr>redirected to</c> <cy>browser auth interfaces</c>.",
            "Abort", "Continue",
            360.f
        );
        pop->setID("info");
        if (auto logout_btnspr = ButtonSprite::create("Logout", "goldFont.fnt", "GJ_button_05.png")) {
            logout_btnspr->setScale(0.7f);
            auto logout = CCMenuItemSpriteExtra::create(logout_btnspr, pop, menu_selector(GitHubAuthPopup::onLogout));
            logout->setID("logout");
            logout->setAnchorPoint({0.5f, 1.f});
            logout->setPositionY(-33.f);
            logout_btnspr->m_BGSprite->setOpacity(0);
            logout_btnspr->m_BGSprite->runAction(CCEaseExponentialIn::create(CCFadeIn::create(2.0f)));
            logout_btnspr->m_label->setOpacity(0);
            logout_btnspr->m_label->runAction(CCEaseExponentialIn::create(CCFadeIn::create(2.0f)));
            if (checkExistence(ghc_file)) pop->m_buttonMenu->addChild(logout);
        }
        pop->show();
    }
    void onLogout(CCObject* btnObj) {
        if (auto logout = dynamic_cast<CCMenuItemSpriteExtra*>(btnObj)) {
            auto logout_btnspr = dynamic_cast<ButtonSprite*>(logout->getNormalImage());
            if (not logout_btnspr) return;
            if (not checkExistence(ghc_file)) return logout_btnspr->setString("Here is no save file anymore...");
            logout_btnspr->setString("Access token save file was deleted!");
            logout_btnspr->setScale(0.625f); 
            ghc::filesystem::remove(ghc_file);
        }
    }
    void onPasteToInput(CCObject* btnObj) {
        auto btn = dynamic_cast<CCNode*>(btnObj);
        auto menu = btn->getParent();
        auto input = dynamic_cast<InputNode*>(menu->getChildByIDRecursive("input"));
        input->setString(utils::clipboard::read());
    };
    void onOpenupBtn(CCObject*) {
        show_info();
    }
    static auto addMyBtn(CCNode* parent_lay, bool animate = false) {
        //item
        auto repoBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("geode.loader/github.png"),
            parent_lay,
            menu_selector(GitHubAuthPopup::onOpenupBtn)
        );
        if (not checkExistence(ghc_file)) {
            if (animate) repoBtn->getNormalImage()->runAction({
                    CCRepeatForever::create(CCSequence::create(
                        CCEaseSineInOut::create(CCScaleTo::create(0.5f, 1.05f)),
                        CCEaseSineInOut::create(CCScaleTo::create(0.5f, 1.f)),
                        nullptr
                    ))
                });
        }
        else repoBtn->setOpacity(60);
        //menu
        auto menu = CCMenu::create(repoBtn, nullptr);
        menu->setPositionX(32.f);
        menu->setPositionY(73.f + menu->getPositionY());
        parent_lay->addChild(menu);
    }
};

class ModListLayer : public CCLayer, public TextInputDelegate {
public:
    GJListLayer* m_list = nullptr;
    CCClippingNode* m_tabsGradientNode = nullptr;
    CCSprite* m_tabsGradientSprite = nullptr;
    CCSprite* m_tabsGradientStencil = nullptr;
    CCLabelBMFont* m_listLabel;
    CCLabelBMFont* m_indexUpdateLabel;
    CCMenu* m_menu;
    CCMenu* m_topMenu;
    CCMenuItemToggler* m_installedTabBtn;
    CCMenuItemToggler* m_downloadTabBtn;
    CCMenuItemToggler* m_featuredTabBtn;
    CCMenuItemSpriteExtra* m_searchBtn;
    CCMenuItemSpriteExtra* m_searchClearBtn;
    CCMenuItemSpriteExtra* m_checkForUpdatesBtn = nullptr;
    CCNode* m_searchBG = nullptr;
    CCTextInputNode* m_searchInput = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;
    CCMenuItemSpriteExtra* m_filterBtn;
    //ModListQuery m_query;
    //ModListDisplay m_display = ModListDisplay::Concise;
    //EventListener<IndexUpdateFilter> m_indexListener;
    static inline ModListLayer* lastCreatedOne;
    void tryCustomSetup(float);
};
class PackSelectLayer : public CCLayer {
public:
    ScrollLayer* m_availableList = nullptr;
    ScrollLayer* m_appliedList = nullptr;
    static inline PackSelectLayer* lastCreatedOne;
    void tryCustomSetup(float);
    void back(CCObject*) {
        //CCMessageBox("asd", __FUNCTION__);
        CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }
    static void openLastCreatedOne() {
        if (not lastCreatedOne) return;
        auto scene = CCScene::create();
        scene->addChild(lastCreatedOne);
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, scene));
    }
};

auto basicRznLayersInit(CCLayer* rtn, cocos2d::SEL_MenuHandler onBtnSel) {
    {
        //setup
        rtn->setKeypadEnabled(true);
        rtn->setTouchEnabled(true);
        CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
        backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
        backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
        backgroundSprite->setAnchorPoint({ 0, 0 });
        backgroundSprite->setColor({ 120, 120, 130 });
        rtn->addChild(backgroundSprite, -2);
        /*SquareShadowCorners*/ {
            auto scale = 1.f;
            auto opacity = 160;
            //SquareShadowCorner1
            CCSprite* SquareShadowCorner1 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
            SquareShadowCorner1->setScaleX(scale);
            SquareShadowCorner1->setOpacity(opacity);
            SquareShadowCorner1->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner1->getContentSize().height);
            SquareShadowCorner1->setAnchorPoint({ 0, 0 });
            rtn->addChild(SquareShadowCorner1, -1);
            //SquareShadowCorner2
            CCSprite* SquareShadowCorner2 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
            SquareShadowCorner2->setScaleX(-scale);
            SquareShadowCorner2->setOpacity(opacity);
            SquareShadowCorner2->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner2->getContentSize().height);
            SquareShadowCorner2->setAnchorPoint({ 0, 0 });
            SquareShadowCorner2->setPositionX(CCDirector::sharedDirector()->getScreenRight());
            rtn->addChild(SquareShadowCorner2, -1);
        };
        /*gauntletCorners*/ {
            //gauntletCorner_001
            CCSprite* gauntletCorner_001 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
            gauntletCorner_001->setPosition({ 0, 0 });
            gauntletCorner_001->setRotation(0);
            gauntletCorner_001->setAnchorPoint({ 0.f,0.f });
            rtn->addChild(gauntletCorner_001);//add gauntletCorner_001
            //gauntletCorner_002
            CCSprite* gauntletCorner_002 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
            gauntletCorner_002->setPosition({
                CCDirector::sharedDirector()->getWinSize().width,
                0.f
                });
            gauntletCorner_002->setScaleX(-1.f);
            gauntletCorner_002->setAnchorPoint({ 0.f,0.f });
            rtn->addChild(gauntletCorner_002);//add gauntletCorner_002
            //gauntletCorner_003
            CCSprite* gauntletCorner_003 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
            gauntletCorner_003->setPosition({
                (CCDirector::sharedDirector()->getWinSize().width),
                (CCDirector::sharedDirector()->getWinSize().height)
                });
            gauntletCorner_003->setRotation(180.f);
            gauntletCorner_003->setAnchorPoint({ 0.f,0.f });
            rtn->addChild(gauntletCorner_003);//add gauntletCorner_003
        };
        /*backBtn*/ {
            auto back = CCMenuItemSpriteExtra::create(
                CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"),
                rtn,
                onBtnSel
            );
            back->setID("back");
            auto menuBack = CCMenu::createWithItem(back);
            menuBack->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
            rtn->addChild(menuBack);
        };
    };
    return rtn;
}

void openLastViewed();

class IssueCommentItem : public CCMenuItem {
public:
    matjson::Value m_json; 
    static auto create(CCNode* parent, matjson::Value json) {
        auto rtn = new IssueCommentItem();
        rtn->m_json = json;
        rtn->init();
        rtn->customSetup(parent);
        return rtn;
    }
    void customSetup(CCNode* parent) {
        this->setContentWidth(parent->getContentWidth());
        //row
        if (auto row = CCNode::create()) {
            this->addChild(row);
            row->setLayout(
                RowLayout::create()
                ->setAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisLineAlignment(AxisAlignment::End)
            );
            row->setContentWidth(parent->getContentWidth());
            //avatar
            auto avatar = CCNode::create();
            if (avatar) {
                row->addChild(avatar);
                avatar->setContentSize({ 30.f, 30.f });
                //
                auto sprite = CCSprite::createWithSpriteFrameName("edit_eDamageSquare_001.png");
                sprite->setAnchorPoint(CCPointZero);
                sprite->setScale(avatar->getContentWidth() / sprite->getContentSize().width);
                avatar->addChild(sprite);
                //
                auto filep = dirs::getTempDir() / "avatars" /("." + m_json["user"]["login"].as_string());
                auto a = [this, sprite, filep, avatar](std::monostate const& asd) {
                    if (not sprite) return;
                    sprite->initWithFile(filep.string().c_str());
                    sprite->setAnchorPoint(CCPointZero);
                    sprite->setScale(avatar->getContentWidth() / sprite->getContentSize().width);
                    };
                auto b = [this, sprite](std::string const& error)
                    {
                    };
                ghc::filesystem::create_directories(dirs::getTempDir() / "avatars");
                web::AsyncWebRequest().fetch(m_json["user"]["avatar_url"].as_string())
                    .into(filep).then(a).expect(b);
            }
            //text
            if (auto text = CCNode::create()) {
                row->addChild(text);
                text->setLayout(
                    ColumnLayout::create()
                    ->setCrossAxisLineAlignment(AxisAlignment::Start)
                    ->setAxisReverse(true)
                    ->setGap(0.f)
                );
                text->setContentWidth(parent->getContentWidth() - avatar->getContentWidth());
                //user
                auto updated_at = m_json["updated_at"].as_string();
                updated_at = string::replace(updated_at, "T", " ");
                updated_at = string::replace(updated_at, "Z", "");
                auto author_association = m_json["author_association"].as_string();
                if (author_association != "NONE") author_association = author_association + ", ";
                else author_association = "";
                auto user = CCLabelTTF::create(
                    fmt::format(
                        "{} | {}created at {}",
                        m_json["user"]["login"].as_string(),
                        author_association,
                        updated_at
                    ).c_str(),
                    "arial", 12.f
                );
                user->setID("user");
                text->addChild(user);
                //menu in end of user text
                if (auto menu = CCMenu::create()) {
                    menu->setPosition(user->getContentSize());
                    menu->setContentHeight(user->getContentHeight());
                    menu->setAnchorPoint({ -0.01f, 1.f });
                    menu->setLayout(
                        RowLayout::create()
                        ->setAxisAlignment(AxisAlignment::Start)
                        ->setAutoScale(false)
                        ->setCrossAxisOverflow(false)
                    );
                    user->addChild(menu);
                    //delete_btn
                    auto edit_delBtn_001 = CCSprite::createWithSpriteFrameName("edit_delBtn_001.png");
                    edit_delBtn_001->setScale(0.5f);
                    auto delete_btn = CCMenuItemSpriteExtra::create(
                        edit_delBtn_001,
                        this,
                        menu_selector(IssueCommentItem::deleteComment)
                    );
                    menu->addChild(delete_btn);
                    //menu update
                    menu->updateLayout();
                }
                //body
                auto body = public_cast(
                    MDTextArea::create(m_json["body"].as_string(), { text->getContentWidth(), 10}),
                    m_content
                );
                body->setVisible(1);
                auto body_container = CCNode::create();
                body_container->addChild(body);
                body_container->setLayout(ColumnLayout::create());
                body_container->setContentHeight(body->getContentHeight());
                body_container->updateLayout();
                text->addChild(body_container);
                //set col height
                text->setContentHeight(user->getContentHeight() + body_container->getContentHeight());
                text->updateLayout();
            }
            //upd
            row->updateLayout();
        }
        //update size
        this->setLayout(RowLayout::create());
        //bg
        CCSprite* bg = CCSprite::create("Ryzen_SquareShadow_001.png"_spr); {
            bg->setID("bg");
            bg->setOpacity(90);
            bg->setScaleX(((this->getContentSize().width + 12) / bg->getContentSize().width));
            bg->setScaleY(((this->getContentSize().height + 8) / bg->getContentSize().height));
            bg->setPosition(this->getContentSize() / 2);
            this->addChild(bg, -1);
        }
        //menu
        if (auto menu = CCMenu::create()) {
        };
    }
    //IssueCommentItem
    void deleteComment(CCObject*) {
        auto a = [this](std::string const& rtn)
            {
                if (auto reload = dynamic_cast<CCMenuItemSpriteExtra*>(CCDirector::get()->m_pRunningScene->getChildByIDRecursive("reload")))
                    reload->activate();
                //asd
                if (this) {
                    auto parent = this->getParent();
                    this->removeFromParentAndCleanup(false);
                    if (parent) parent->updateLayout();
                };
            };
        auto b = [this](std::string const& rtn)
            {
                auto message = rtn;
                auto asd = geode::createQuickPopup(
                    "Request exception",
                    message,
                    "Nah", nullptr, 420.f, nullptr, false
                );
                asd->show();
            };
        web::AsyncWebRequest()
            ghapiauth
            .method("DELETE")
            .fetch(m_json["url"].as_string())
            .text().then(a).expect(b);
    }
};

class IssueCommentsLayer : public CCLayer, DynamicScrollDelegate, FLAlertLayerProtocol, TextInputDelegate {
public:
    void textChanged(CCTextInputNode* p0) {
        auto endl_filtered = string::replace(p0->getString().data(), "\\n", "\n");
        if (std::string(p0->getString().c_str()).find("\\n") != std::string::npos) {
            p0->setString(endl_filtered.data());
        }
        //p0->setString();
        //log::debug("{}(text:{}, m_fontValue1={})", __FUNCTION__, p0->getString(), p0->m_fontValue2);
        auto md_prev = dynamic_cast<MDTextArea*>(CCDirector::get()->m_pRunningScene->getChildByIDRecursive("md_prev"));
        if (md_prev) {
            md_prev->setString(endl_filtered.data());
            md_prev->getScrollLayer()->scrollLayer(9999.f);
        }
    }
    void FLAlert_Clicked(FLAlertLayer* p0, bool p1) {
        //info"Continue"
        if (p0->getID() == "post_comment_popup" and p1) {
            //data
            auto data = std::string("");
            auto input = dynamic_cast<TextInput*>(p0->getChildByIDRecursive("input"));
            if (input) data = input->getString();
            //go
            uploadComment(data);
        };
    }
    //data getting
    matjson::Value data() {
        auto json_container = dynamic_cast<CCLabelBMFont*>(getChildByID("json_container"));//ini_container
        auto json_str = std::string(json_container->getString());
        return matjson::parse(json_str);
    }
    matjson::Value issue_data() {
        auto issue_json_base64 = data()["issue_json_base64"].as_string();
        auto issue_json = ZipUtils::base64URLDecode(issue_json_base64);
        return matjson::parse(issue_json);
    }
    std::string data_strkey(std::string asd) {
        if (not data().contains(asd.data())) return std::string("cant find key");
        return data()[asd.data()].as_string();
    }
    //others
    static auto create(matjson::Value json, bool load = true) {
        auto rtn = new IssueCommentsLayer;
        rtn->init();
        basicRznLayersInit(rtn, menu_selector(IssueCommentsLayer::onBtn));
        GitHubAuthPopup::addMyBtn(rtn, true);
        //json
        auto json_container = CCLabelBMFont::create(json.dump().data(), "chatFont.fnt");
        json_container->setVisible(0);
        json_container->setID("json_container");
        rtn->addChild(json_container, 999);
        //customSetup
        rtn->customSetup();
        //rtn->load
        if (load) rtn->load();
        return rtn;
    }
    float scroll_gap = 12.f;
    void customSetup() {
        /* scroll lay */ {
            auto paddingx = 190.f;
            auto paddingt = 10.f;
            auto scroll_size = CCSize(CCDirector::get()->getScreenRight() - paddingx - 4, CCDirector::get()->getScreenTop() - paddingt);
            auto scroll = geode::ScrollLayer::create(scroll_size);
            {
                scroll->setID("scroll");
                scroll->enableScrollWheel();
                scroll->m_cutContent = (false);
                scroll->m_contentLayer->setLayout(
                    ColumnLayout::create()
                    ->setGap(scroll_gap)
                    ->setAxisReverse(true)
                    ->setAxisAlignment(AxisAlignment::End)
                );
                scroll->setPositionX(paddingx / 2 + 2);
                this->addChild(scroll, -1);
                //shadow_cornerl
                CCSprite* shadow_cornerl = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                shadow_cornerl->setID("shadow_cornerl");
                shadow_cornerl->setOpacity(60);
                shadow_cornerl->setScaleY(-6.f);
                shadow_cornerl->setScaleX(-0.1f);
                shadow_cornerl->setAnchorPoint({ -0.3f, 1.f });
                scroll->addChild(shadow_cornerl, -1);
                //shadow_cornerr
                CCSprite* shadow_cornerr = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                shadow_cornerr->setID("shadow_cornerr");
                shadow_cornerr->setOpacity(60);
                shadow_cornerr->setScaleY(6.f);
                shadow_cornerr->setScaleX(0.1f);
                shadow_cornerr->setPositionX(scroll_size.width);
                shadow_cornerr->setAnchorPoint({ -0.3f, 0.f });
                scroll->addChild(shadow_cornerr, -1);
                //shadow_cornerb
                CCSprite* shadow_cornerb = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                shadow_cornerb->setID("shadow_cornerb");
                shadow_cornerb->setOpacity(90);
                shadow_cornerb->setScaleY(((scroll_size.width + 20) / shadow_cornerb->getContentSize().width));
                shadow_cornerb->setScaleX(CCDirector::get()->getScreenTop() / shadow_cornerb->getContentHeight());
                shadow_cornerb->setRotation(-90.f);
                shadow_cornerb->setPositionX(scroll_size.width / 2);
                shadow_cornerb->setAnchorPoint({ 0.f, 0.5f });
                scroll->addChild(shadow_cornerb, -1);
            };
        }
        /* down right buttons */ {
            CCMenu* menu = CCMenu::create();
            if (menu) {
                menu->setPosition(CCDirector::get()->getScreenRight(), 0.f);
                menu->setAnchorPoint({ 1.f, 0.f });
                menu->setLayout(
                    ColumnLayout::create()
                    ->setAxisAlignment(AxisAlignment::Start)
                    ->setGap(0.f)
                );
                this->addChild(menu);
            };
            //reloadmodsbtn
            if (auto sprite = CCSprite::create("Ryzen_ReloadBtn_001.png"_spr)) {
                CCMenuItemSpriteExtra* reload = CCMenuItemSpriteExtra::create(
                    sprite, this, menu_selector(IssueCommentsLayer::onBtn)
                );
                reload->setID("reload");
                reload->getNormalImage()->setScale(0.7f);
                menu->addChild(reload);
            };
            //post
            if (auto sprite = CCSprite::create("Ryzen_CommentsBtn_001.png"_spr)) {
                CCMenuItemSpriteExtra* post = CCMenuItemSpriteExtra::create(
                    sprite, this, menu_selector(IssueCommentsLayer::onBtn)
                );
                post->setID("post");
                //reload->getNormalImage()->setScale(0.7f);
                menu->addChild(post);
            };
            menu->updateLayout();
        }
    }
    void setupComments(matjson::Value comments_json) {
        if (not dynamic_cast<IssueCommentsLayer*>(this)) return;
        auto scroll = dynamic_cast<ScrollLayer*>(this->getChildByID("scroll"));
        auto content = scroll->m_contentLayer;
        content->removeAllChildren();
        content->setContentHeight(0.f);
        for (auto comment : comments_json.as_array()) {
            auto item = IssueCommentItem::create(content, comment);
            content->setContentHeight(//make content layer longer
                content->getContentHeight() + item->getContentHeight() + (scroll_gap / 2) + 6
            );
            content->addChild(item);
        }
        //fix some shit goes when content smaller than scroll
        if (scroll->m_contentLayer->getContentSize().height < scroll->getContentSize().height) {
            scroll->m_contentLayer->setContentSize({
                scroll->m_contentLayer->getContentSize().width,
                scroll->getContentSize().height
                });
        }
        content->updateLayout();
    }
    void load() {
        auto local_issues_file = (ghc::filesystem::path(data_strkey("workindir")) / "comments.json");
        auto local_issues = std::ifstream(local_issues_file);
        if (local_issues.is_open()) {
            std::string data(
                (std::istreambuf_iterator<char>(local_issues)), 
                std::istreambuf_iterator<char>()
            );
            return setupComments(matjson::parse(data));
        }
        auto ntfy = geode::Notification::create("Loading comments...", NotificationIcon::Loading, 15.f);
        ntfy->show();
        auto a = [this, ntfy](matjson::Value const& catgirls) {
            auto local_issues_save = (ghc::filesystem::path(data_strkey("workindir")) / "comments.json");
            ghc::filesystem::create_directories(local_issues_save.parent_path());
            std::ofstream(local_issues_save.string().c_str()) << catgirls.dump(); 
            setupComments(catgirls);
            ntfy->setString("Comments loaded!");
            ntfy->setIcon(NotificationIcon::Success);
            ntfy->setTime(1.0f);
            };
        auto b = [this, ntfy](std::string const& error)
            {// something went wrong with our web request Q~Q
                ntfy->hide();
                auto message = error;
                auto asd = geode::createQuickPopup(
                    "Request exception",
                    message,
                    "Nah", nullptr, 420.f, nullptr, false
                );
                asd->m_scene = this;
                asd->show();
            };
        web::AsyncWebRequest()
            ghapiauth.fetch(issue_data()["comments_url"].as_string())
            .json().then(a).expect(b);
    }
    //other shit
    void uploadComment(std::string data) {
        auto body = matjson::parse("{\"body\": \"\"}");
        body["body"] = data;
        auto a = [this](std::string const& rtn)
            {
                if (string::contains(rtn, "\"body\":")) {
                    if (auto reload = dynamic_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("reload")))
                        reload->activate();
                    return;
                }
                auto message = rtn;
                auto asd = geode::createQuickPopup(
                    "Request",
                    message,
                    "Nah", nullptr, 420.f, nullptr, false
                );
                asd->m_scene = this;
                asd->show();
            };
        auto b = [this](std::string const& rtn)
            {
                auto message = rtn;
                auto asd = geode::createQuickPopup(
                    "Request exception",
                    message,
                    "Nah", nullptr, 420.f, nullptr, false
                );
                asd->m_scene = this;
                asd->show();
            };
        web::AsyncWebRequest()
            ghapiauth
            .body(body)
            .post(issue_data()["comments_url"].as_string())
            .text().then(a).expect(b);
    }
    void onBtn(CCObject* pCCObject) {
        auto what = dynamic_cast<CCNode*>(pCCObject);
        if (not what) return;
        if (what->getID() == "back") keyBackClicked();
        if (what->getID() == "post") {
            auto pop = FLAlertLayer::create(
                this,
                "Create Comment",
                "\n \n \n \n \n ",
                "Close", "Create",
                380.f
            );
            //md_prev
            auto md_prev = MDTextArea::create("# input and preview here...\nJust tap on me and type your text!\n\nAlso you can type <co>`\\n`</c> and it will be replaced with newline char automatically. And remember, in **Markdown** you write two newlines to start new paragraph!", CCSize(290.f, 112.f));
            md_prev->setID("md_prev");
            md_prev->setPositionY(86.f);
            pop->m_buttonMenu->addChild(md_prev, 1);
            //input
            auto input = TextInput::create(md_prev->getContentWidth(), "", "chatFont.fnt");
            input->setID("input");
            input->hideBG();
            input->setContentHeight(md_prev->getContentHeight());
            input->getInputNode()->setContentHeight(md_prev->getContentHeight());
            input->getInputNode()->m_placeholderLabel->setOpacity(0);
            input->getInputNode()->m_cursor->setOpacity(0);
            input->getInputNode()->m_filterSwearWords = (0);
            input->getInputNode()->m_allowedChars = (
                " !\"#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
                );
            input->setDelegate(this);
            input->setPosition(md_prev->getPosition());
            pop->m_buttonMenu->addChild(input);
            //last popup setup
            handleTouchPriority(pop);
            pop->setID("post_comment_popup");
            pop->show();
        };
        if (what->getID() == "reload") {
            auto jsonfilepath = ghc::filesystem::path(data_strkey("workindir")) / "comments.json";
            if (checkExistence(jsonfilepath))
                ghc::filesystem::remove(jsonfilepath);
            load();
        };
    }
    static void openMe(matjson::Value issue_json) {
        auto scene = CCScene::create();
        auto pIssueCommentsLayer = IssueCommentsLayer::create(issue_json);
        scene->addChild(pIssueCommentsLayer, 0, pIssueCommentsLayer->issue_data()["number"].as_int());
        CCDirector::sharedDirector()->pushScene(scene);
    };
    void keyBackClicked() {
        CCDirector::sharedDirector()->popScene();
    }
};

class ModViewLayer : public CCLayer {
public:
    static auto create(matjson::Value json) {
        auto rtn = new ModViewLayer;
        rtn->init();
        basicRznLayersInit(rtn, menu_selector(ModViewLayer::onBtn));
        //json
        auto json_container = CCLabelBMFont::create(json.dump().data(), "chatFont.fnt");
        json_container->setVisible(0);
        json_container->setID("json_container");
        rtn->addChild(json_container, 999);
        //customSetup call
        rtn->customSetup();
        return rtn;
    }
    void customSetup() {
        /* top center card */ {
            auto menu = CCMenu::create();
            menu->setID("card");
            addChild(menu);
            menu->setPositionX(CCDirector::get()->getScreenRight() / 2);
            menu->setPositionY(CCDirector::get()->getScreenTop() - 35.f);
            menu->setContentWidth(menu->getContentWidth() - 75.f);
            menu->setScale(0.85f);
            menu->setLayout(
                RowLayout::create()
                ->setGap(15.f)
                ->setCrossAxisOverflow(false)
            );
            /*some fun stuff*/ {
                //logo
                auto logo = CCSprite::create((ghc::filesystem::path(strKeyOfdata("workindir")) / ".logo").string().c_str());
                if (!logo) logo = CCSprite::createWithSpriteFrameName("deleteFilter_none_001.png");
                if (logo) {
                    auto icon_size = CCSize(68.f, 68.f);
                    auto real_size = logo->getContentSize();
                    logo->setID("logo");
                    if (real_size.height > icon_size.height) {
                        logo->setScale(icon_size.height / real_size.height);
                    }
                    else {
                        logo->setScale(icon_size.width / real_size.width);
                    }
                    logo->setAnchorPoint(CCPointZero);
                    auto container = CCNode::create();
                    container->addChild(logo);
                    container->setID("icon_container");
                    container->setContentSize(icon_size);
                    menu->addChild(container);
                };
                //text_part_container
                if (auto text_part_container = CCMenu::create()) {
                    auto size = CCSize(320.f, 68.f);
                    auto parent = text_part_container;
                    parent->setID("text_part_container");
                    parent->setContentSize(size);
                    parent->setLayout(
                        ColumnLayout::create()
                        ->setCrossAxisLineAlignment(AxisAlignment::Start)
                        ->setCrossAxisAlignment(AxisAlignment::Start)
                        ->setAxisAlignment(AxisAlignment::Even)
                        ->setAxisReverse(true)
                        ->setCrossAxisOverflow(false)
                    );
                    /*title*/ {
                        auto title = CCLabelTTF::create(
                            strKeyOfdata("title").data(),
                            "arial", 18.f
                        );
                        parent->addChild(title);
                    }
                    /*devs*/ {
                        auto label = CCLabelTTF::create(strKeyOfdata("devs").data(), "arial", 14.f);
                        parent->addChild(label);
                    }
                    /*description*/ {
                        auto strDesc = strKeyOfdata("desc").data();
                        TextArea* shit = TextArea::create(
                            strDesc, "chatFont.fnt", 1.0f, 600.f, { 0.0f, 0.0f }, 10.0f, false
                        );
                        auto size_resolve = CCLabelBMFont::create(strDesc, "chatFont.fnt");
                        auto desc = cocos::getChild(cocos::getChild(shit, 0), 0);
                        desc->removeFromParentAndCleanup(false);
                        //desc->setAnchorPoint({ 0.0f, -2.f });
                        parent->addChild(desc);
                    }
                    parent->updateLayout();
                    menu->addChild(parent);
                };
                //download_part
                if (auto download_part = CCMenu::create()) {
                    auto size = CCSize(152.f, 68.f);
                    auto parent = download_part;
                    parent->setID("download_part");
                    parent->setContentSize(size);
                    parent->setLayout(
                        ColumnLayout::create()
                        ->setCrossAxisLineAlignment(AxisAlignment::Center)
                        ->setAxisAlignment(AxisAlignment::Even)
                        ->setAxisReverse(true)
                    );
                    if (auto downloadBtn = ButtonSprite::create("Download", "goldFont.fnt", "GJ_button_05.png")) {
                        if (checkExistence(ghc::filesystem::path(strKeyOfdata("download_path")))) {
                            downloadBtn->m_label->setString("Update");
                            downloadBtn->m_label->setScale(
                                (downloadBtn->m_BGSprite->getContentWidth() - 15) / downloadBtn->m_label->getContentWidth()
                            );
                        }
                        if (downloadBtn->m_label->getScale() > 0.9f) downloadBtn->m_label->setScale(0.9f);
                        downloadBtn->setScale(0.8f);
                        downloadBtn->setID("downloadBtn");
                        auto item = CCMenuItemSpriteExtra::create(downloadBtn, this, menu_selector(ModViewLayer::onBtn));
                        item->setID("download");
                        parent->addChild(item);
                    }
                    //statsContainerMenu
                    if (DATA()["size"].as_int() != 0) {
                        CCMenu* statsContainerMenu = CCMenu::create();
                        parent->addChild(statsContainerMenu);
                        statsContainerMenu->setID("statsContainerMenu");
                        statsContainerMenu->setContentSize(size);
                        statsContainerMenu->setLayout(
                            RowLayout::create()
                            ->setGap(20.f)
                            ->setGrowCrossAxis(true)
                            ->setAxisAlignment(AxisAlignment::Center)
                        );
                        /*download_count*/ {
                            auto download_count = CCLabelTTF::create(
                                abbreviateNumber(DATA()[("downloads")].as_int()).c_str(),
                                "arial",
                                12.f
                            );
                            download_count->setID("download_count");
                            download_count->setAnchorPoint({ 0.f, 0.5f });
                            download_count->setScale(0.65f);
                            download_count->addChild(CCSprite::createWithSpriteFrameName("GJ_sDownloadIcon_001.png"), 0, 521);
                            download_count->getChildByTag(521)->setAnchorPoint({ 1.0f, 0.0f });
                            statsContainerMenu->addChild(download_count);
                        }
                        /*size*/ {
                            auto size = CCLabelTTF::create(
                                convertSize(DATA()[("size")].as_int()).c_str(),
                                "arial",
                                12.f
                            );
                            statsContainerMenu->addChild(size);
                            size->setID("size");
                            size->setAnchorPoint({ 0.f, 0.5f });
                            size->setScale(0.65f);
                            size->addChild(CCSprite::createWithSpriteFrameName("geode.loader/changelog.png"), 0, 521);
                            size->getChildByTag(521)->setAnchorPoint({ 1.10f, 0.0f });
                            size->getChildByTag(521)->setScale(0.6f);
                        };
                        statsContainerMenu->updateLayout();
                    }
                    parent->updateLayout();
                    menu->addChild(parent);
                }
            }
            menu->updateLayout();
        };
        /* br buttons */ {
            CCMenu* menu = CCMenu::create();
            if (menu) {
                menu->setPosition(CCDirector::get()->getScreenRight(), 0.f);
                menu->setAnchorPoint({ 1.f, 0.f });
                menu->setLayout(
                    ColumnLayout::create()
                    ->setAxisAlignment(AxisAlignment::Start)
                    ->setGap(0.f)
                );
                this->addChild(menu);
            };
            //reloadmodsbtn
            if (auto sprite = CCSprite::create("Ryzen_ReloadBtn_001.png"_spr)) {
                CCMenuItemSpriteExtra* reload = CCMenuItemSpriteExtra::create(
                    sprite, this, menu_selector(ModViewLayer::onBtn)
                );
                reload->setID("reload");
                reload->getNormalImage()->setScale(0.7f);
                menu->addChild(reload);
            };
            //comments
            if (auto sprite = CCSprite::create("Ryzen_CommentsBtn_001.png"_spr)) {
                CCMenuItemSpriteExtra* comments = CCMenuItemSpriteExtra::create(
                    sprite, this, menu_selector(ModViewLayer::onBtn)
                );
                comments->setID("comments");
                //reload->getNormalImage()->setScale(0.7f);
                menu->addChild(comments);
            };
            menu->updateLayout();
        }
        /* cr buttons */ {
            CCMenu* menu = CCMenu::create();
            if (menu) {
                menu->setPosition(32.f, CCDirector::get()->getScreenTop() / 2);
                menu->setAnchorPoint({ 0.5f, 0.5f });
                menu->setLayout(
                    ColumnLayout::create()
                    ->setAxisAlignment(AxisAlignment::Center)
                    ->setGap(6.f)
                );
                this->addChild(menu);
            };
            //github
            if (auto github = CCSprite::createWithSpriteFrameName("geode.loader/github.png")) {
                CCMenuItemSpriteExtra* github_item = CCMenuItemSpriteExtra::create(
                    github, this, menu_selector(ModViewLayer::onBtn)
                );
                github_item->setID("github");
                menu->addChild(github_item);
            };
            menu->updateLayout();
        }
        /* md */ {
            auto pMDTextArea = MDTextArea::create(
                ZipUtils::base64URLDecode(strKeyOfdata("body_base64")),
                { this->getContentWidth() - 140.f, this->getContentHeight() - 80.f, }
            );
            pMDTextArea->setID("pMDTextArea");
            pMDTextArea->setPositionX(this->getContentWidth() / 2);
            pMDTextArea->setAnchorPoint({ 0.5f, 0.f });
            addChild(pMDTextArea, -1, 85290);
            //bg
            auto pMDTextArea_BG = public_cast(pMDTextArea, m_bgSprite);
            pMDTextArea_BG->setPositionY(pMDTextArea_BG->getContentHeight() / 2);
            pMDTextArea_BG->setAnchorPoint({ 0.5f, 1.f });
            pMDTextArea_BG->setContentHeight(pMDTextArea_BG->getContentHeight() + 20.f);
            pMDTextArea_BG->setOpacity(90.f);
        };
    }
    //data gettinga
    matjson::Value DATA() {
        auto json_container = dynamic_cast<CCLabelBMFont*>(
            this->getChildByIDRecursive("json_container")
            );
        auto json_str = std::string(json_container->getString());
        return matjson::parse(json_str);
    }
    matjson::Value ISSUE_DATA() {
        auto issue_json_base64 = DATA()["issue_json_base64"].as_string();
        auto issue_json = ZipUtils::base64URLDecode(issue_json_base64);
        return matjson::parse(issue_json);
    }
    std::string strKeyOfdata(std::string asd) {
        if (not DATA().contains(asd.data())) return std::string("cant find key");
        return DATA()[asd.data()].as_string();
    }
    //other shit
    void onBtn(CCObject* pCCObject) {
        auto what = dynamic_cast<CCMenuItem*>(pCCObject);
        if (not what) return;
        if (what->getID() == "back") keyBackClicked();
        if (what->getID() == "github") {
            web::openLinkInBrowser(strKeyOfdata("github_page_link"));
        };
        if (what->getID() == "comments") {
            IssueCommentsLayer::openMe(DATA());
        }
        if (what->getID() == "reload") {
            if (checkExistence(ghc::filesystem::path(strKeyOfdata("workindir")) / "main.json"))
                remove_dir(ghc::filesystem::path(strKeyOfdata("workindir")));
            auto issue_json_text = ZipUtils::base64URLDecode(strKeyOfdata("issue_json_base64"));
            keyBackClicked();
            openLastViewed();
        };
        if (what->getID() == "download") {
            //endpoint
            auto endpoint = std::string(strKeyOfdata("download_link"));
            auto path = ghc::filesystem::path(strKeyOfdata("download_path"));
            //Popup
            auto pop = createQuickPopup(
                "Download mod?",
                "\n \n \n \n \n \n \n",
                "Abort", "Start",
                400.f,
                [this, endpoint, path, what](auto, bool btn2) {
                    if (not btn2) return;
                    auto downloadBtn = reinterpret_cast<ButtonSprite*>(this->getChildByIDRecursive("downloadBtn"));
                    if (not downloadBtn) return;
                    web::AsyncWebRequest()ghapiauth.fetch(endpoint).into(path)
                        .then(
                            [this, downloadBtn, path](std::monostate const& who) {
                                downloadBtn->m_label->setString("Update");
                                downloadBtn->m_label->setScale(
                                    (downloadBtn->m_BGSprite->getContentWidth() - 15) / downloadBtn->m_label->getContentWidth()
                                );
                                if (downloadBtn->m_label->getScale() > 0.9f) downloadBtn->m_label->setScale(0.9f);
                                if (string::contains(path.string(), "geode.texture-loader")) {
                                    FLAlertLayer* asd = geode::createQuickPopup(
                                        "Pack Downloaded",
                                        "Open pack select menu?",
                                        "Later", "Open",
                                        [path](auto, bool btn2) {
                                            if (btn2) PackSelectLayer::openLastCreatedOne();
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
                            })
                        //utils::MiniFunction<void(SentAsyncWebRequest&, double, double)>;
                                .progress(
                                    [downloadBtn](auto, double d1, double d2) {
                                        if (d1 <= 0) return;
                                        if (d2 <= 0) return;
                                        downloadBtn->m_label->setString(
                                            fmt::format(
                                                "{} of {}",
                                                abbreviateNumber(d1),
                                                abbreviateNumber(d2)
                                            ).c_str());
                                        downloadBtn->m_label->setScale(
                                            (downloadBtn->m_BGSprite->getContentWidth() - 15) / downloadBtn->m_label->getContentWidth()
                                        );
                                        if (downloadBtn->m_label->getScale() > 0.9f) downloadBtn->m_label->setScale(0.9f);
                                    })
                                .expect(
                                    [this, endpoint, downloadBtn](std::string const& what) {
                                        auto asd = geode::createQuickPopup(
                                            "Request exception",
                                            what + "\n" + endpoint,
                                            "Nah", nullptr, 420.f, nullptr, false
                                        );
                                        asd->m_scene = this;
                                        asd->show();
                                    });
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
                if (auto delte_btnspr = ButtonSprite::create("Delete Installed Mod", "goldFont.fnt", "GJ_button_05.png")) {
                    delte_btnspr->setScale(0.7f);
                    auto delete_item = CCMenuItemSpriteExtra::create(delte_btnspr, this, menu_selector(ModViewLayer::onBtn));
                    delete_item->setID("delete");
                    delete_item->setAnchorPoint({ 0.5f, 1.f });
                    delete_item->setPositionY(-33.f);
                    if (checkExistence(path)) pop->m_buttonMenu->addChild(delete_item);
                }
            };
        };
        if (what->getID() == "delete") {
            AppDelegate::sharedApplication()->trySaveGame(false);
            auto path = ghc::filesystem::path(strKeyOfdata("download_path"));
            if (not checkExistence(path)) return;
            log::info("deleting mod at {}", path);
            #ifdef GEODE_IS_WINDOWS
            auto modulename = path.string().c_str();
            auto handle = GetModuleHandle(modulename);
            if (handle) {
                log::warn(
                    "calling FreeLibrary for handle (0x{}) of module by name \"{}\"",
                    (std::stringstream() << std::hex << (int)handle).str(),
                    modulename
                );
                FreeLibrary(handle);
            }
            #endif
            if (ghc::filesystem::remove(path))
                log::warn("removed file at {}", path.string());
            Notification::create(strKeyOfdata("title") + " got deleted!")->show();
            what->setID("reload");
            what->activate();
        }
    }
    //ModViewLayer
    static void openMe(matjson::Value data) {
        auto scene = CCScene::create();
        auto pModViewLayer = ModViewLayer::create(data);
        scene->addChild(pModViewLayer, 0, pModViewLayer->ISSUE_DATA()["number"].as_int());
        CCDirector::sharedDirector()->pushScene(scene);
    };
    void keyBackClicked() {
        CCDirector::sharedDirector()->popScene();
    }
};

class ModLoadingLayer : public CCLayer {
public:
    //data
    enum data {
        issue, issue_body_ini
        , repo
        , release, asset
        , working_dir_str
    };
    void addTextContainer(data type, std::string data) {
        auto container = CCLabelBMFont::create(data.data(), "chatFont.fnt");
        container->setVisible(0);
        container->setID(fmt::format("container{}", (int)type));
        this->addChild(container, 999);
    }
    std::string setData(data type, std::string data) {
        auto container = dynamic_cast<CCLabelBMFont*>(this->getChildByID(fmt::format("container{}", (int)type)));
        if (container) container->setString(data.data());
        return container ? container->getString() : "CANT GET DATA CONTAINER NODE";
    }
    std::string getData(data type) {
        auto container = dynamic_cast<CCLabelBMFont*>(this->getChildByID(fmt::format("container{}", (int)type)));
        return container ? container->getString() : "CANT GET DATA CONTAINER NODE";
    }
    auto getIniData(data type) {
        auto INI = new CSimpleIni;
        INI->LoadData(getData(type));
        return INI;
    }
    matjson::Value getJsonData(data type) {
        return matjson::parse(getData(type));
    }
    auto working_dir(std::string file = "") {
        ghc::filesystem::path working_dir = getData(data::working_dir_str);
        ghc::filesystem::create_directories(working_dir);
        if (not file.empty()) return working_dir / file;
        return working_dir;
    }
    //a
    static auto create(matjson::Value IssueJson) {
        auto rtn = new ModLoadingLayer;
        rtn->init();
        basicRznLayersInit(rtn, menu_selector(ModLoadingLayer::onBtn));
        /*textcontainers*/ {
            rtn->addTextContainer(data::issue, IssueJson.dump());
            rtn->addTextContainer(data::issue_body_ini, IssueJson["body"].as_string());
            rtn->addTextContainer(data::repo, "{}");
            rtn->addTextContainer(data::release, "{}");
            rtn->addTextContainer(data::asset, "{}");
            rtn->addTextContainer(
                data::working_dir_str,
                (ryzen_dir / "mods" /
                    fmt::format("{}[{}]", IssueJson["number"].as_int(), IssueJson["title"].as_string())
                    ).string()
            );
        }
        //log
        auto pMDTextArea = geode::MDTextArea::create(
            "", 
            CCSize(
                rtn->getContentSize().width - 70,
                rtn->getContentSize().height - 20
            )
        );
        pMDTextArea->setID("pMDTextArea");
        pMDTextArea->setPosition(rtn->getContentSize() / 2);
        rtn->addChild(pMDTextArea, -1);
        //asd
        auto loadingLabel = CCLabelBMFont::create("Loading...", "bigFont.fnt");
        loadingLabel->setPosition(rtn->getContentSize() / 2);
        rtn->addChild(loadingLabel, 1, 2319);
        return rtn;
    }
    void loadRepo(int step = 0, std::string owerwrite_endpoint = "") {
        std::string main_logo = getIniData(data::issue_body_ini)->GetValue("main", "logo", "na");
        auto repo_owner = getIniData(data::issue_body_ini)->GetValue("repo", "owner", "na");
        auto repo_name = getIniData(data::issue_body_ini)->GetValue("repo", "name", "na");
        auto release_tar = getIniData(data::issue_body_ini)->GetValue("release", "tar", "latest");
        //repo_json
        if (step == 0) {
            log("## step0: load repo");
            log(fmt::format("repo_owner = {}", repo_owner));
            log(fmt::format("repo_name = {}", repo_name));
            auto api_url = fmt::format(
                "https://api.github.com/repos/{}/{}"
                , repo_owner, repo_name
            );
            log(fmt::format("api_url = {}", api_url.data()));
            //then
            auto then = 
                [this](matjson::Value const& catgirl) {
                setData(data::repo, catgirl.dump());
                log("```\nloaded :D\n```");
                loadRepo(1);
                };
            //expect
            auto expect =
                [this](std::string const& what) {
                log("## <cr>" + what + "</c>");
                };
            web::AsyncWebRequest()ghapiauth.fetch(api_url).json().then(then).expect(expect);
        };
        //releases
        if (step == 1) {
            log("## step1: load release");
            log(fmt::format("repo_owner = {}", repo_owner));
            log(fmt::format("repo_name = {}", repo_name));
            log(fmt::format("release_tar = {}", release_tar));
            auto api_url = fmt::format(
                "https://api.github.com/repos/{}/{}/releases/{}"
                , repo_owner, repo_name, release_tar
            );
            log(fmt::format("api_url = {}", api_url.data()));
            //then
            auto then = 
                [this](matjson::Value const& catgirl) {
                //release
                setData(data::release, catgirl.dump());
                //asset
                if (getJsonData(data::release).contains("assets")) {
                    auto assets = getJsonData(data::release)["assets"].as_array();
                    auto file = getIniData(data::issue_body_ini)->GetValue("release", "file", "mod.geode");
                    for (auto asset : assets) {
                        if (asset["name"].as_string() == file) setData(data::asset, asset.dump());
                    }
                }
                log("```\nloaded :D\n```");
                loadRepo(2);
                };
            //expect
            auto expect =
                [this](std::string const& what) {
                log("## <cr>" + what + "</c>");
                };
            web::AsyncWebRequest()ghapiauth.fetch(api_url).json().then(then).expect(expect);
        };
        //logo
        if (step == 2) {
            auto default_branch = getJsonData(data::repo)["default_branch"].as_string();
            log("## step2: download logo");
            log(fmt::format("repo_owner = {}", repo_owner));
            log(fmt::format("repo_name = {}", repo_name));
            log(fmt::format("default_branch = {}", default_branch));
            auto file_url = fmt::format(
                "https://raw.githubusercontent.com/{}/{}/{}/.logo"
                , repo_owner, repo_name, default_branch
            );
            if (not owerwrite_endpoint.empty()) file_url = owerwrite_endpoint;
            log(fmt::format("file_url = {}", file_url.data()));
            //expect
            auto expect =
                [this, file_url](std::string const& what) {
                log("## <cr>" + what + "</c>");
                if (string::contains(file_url, ".logo"))
                    loadRepo(2, string::replace(file_url, ".logo", "pack.png"));
                if (string::contains(file_url, "pack.png"))
                    loadRepo(2, getJsonData(data::repo)["owner"]["avatar_url"].as_string());
                };
            //then
            auto then =
                [this, expect](std::monostate const& a) {
                if (not CCSprite::create(working_dir(".logo").string().data())) expect("Bad image downloaded");
                log("```\nloaded :D\n```");
                loadRepo(3);
                };
            web::AsyncWebRequest()ghapiauth.fetch(file_url).into(working_dir(".logo")).then(then).expect(expect);
        }
        //FINAL OF LOADING REPO
        if (step == 3) {
            log("# FINAL OF LOADING REPO");
            log("setting data for mod view layer");
            log("values: workindir, title, devs, desc, size, downloads, body_base64, issue_json_base64, download_link, download_path");
            log("section: main");
            //jsons and other text files
            auto default_branch = getJsonData(data::repo)["default_branch"].as_string();
            log(fmt::format("repo_owner = {}", repo_owner));
            log(fmt::format("repo_name = {}", repo_name));
            log(fmt::format("default_branch = {}", default_branch));
            auto readmemd_url = fmt::format(
                "https://raw.githubusercontent.com/{}/{}/{}/README.md"
                , repo_owner, repo_name, default_branch
            );
            log(fmt::format("readmemd_url = {}", readmemd_url));
            auto aboutmd_url = fmt::format(
                "https://raw.githubusercontent.com/{}/{}/{}/about.md"
                , repo_owner, repo_name, default_branch
            );
            log(fmt::format("aboutmd_url = {}", aboutmd_url));
            auto modjson_url = fmt::format(
                "https://raw.githubusercontent.com/{}/{}/{}/mod.json"
                , repo_owner, repo_name, default_branch
            );
            log(fmt::format("modjson_url = {}", modjson_url));
            auto packjson_url = fmt::format(
                "https://raw.githubusercontent.com/{}/{}/{}/pack.json"
                , repo_owner, repo_name, default_branch
            );
            log(fmt::format("packjson_url = {}", packjson_url));
            //readme
            log("loading readme... ", "");
            auto readme = web::fetch(readmemd_url).value_or("");
            log(fmt::format("size: {}", readme.size()));
            //about
            log("loading about... ", "");
            auto about = web::fetch(aboutmd_url).value_or("");
            log(fmt::format("size: {}", about.size()));
            //mod
            log("loading mod.json... ", "");
            auto mod = web::fetchJSON(modjson_url).value_or("");
            log(fmt::format("is good: {}", mod.contains("id")));
            //pack
            log("loading pack.json... ", "");
            auto pack = web::fetchJSON(packjson_url).value_or("");
            log(fmt::format("is good: {}", pack.contains("id")));
            //jsonsetup
            auto json = matjson::parse(
                "{                                  \
                    \"workindir\": \"\",            \
                    \"title\": \"\",                \
                    \"devs\": \"\",                 \
                    \"desc\": \"\",                 \
                    \"size\": 0,                    \
                    \"downloads\": 0,               \
                    \"body_base64\": \"\",          \
                    \"issue_json_base64\": \"\",    \
                    \"download_link\": \"\",        \
                    \"download_path\": \"\",        \
                    \"github_page_link\": \"\"      \
                }"
            );
            /*workindir*/ {
                auto val = "workindir";
                std::string set_to = working_dir().string();
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*title*/ {
                auto val = "title";
                std::string set_to = getJsonData(issue)["title"].as_string();
                if (mod.contains("name")) set_to = mod["name"].as_string();
                if (pack.contains("name")) set_to = pack["name"].as_string();
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*devs*/ {
                auto val = "devs";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "publisher", "");
                if (pack.contains("author")) set_to = pack["author"].as_string();
                if (pack.contains("authors")) set_to = pack["authors"].dump();
                if (mod.contains("developer")) set_to = mod["developer"].as_string();
                if (mod.contains("developers")) set_to = mod["developers"].dump();
                set_to = string::replace(set_to, "[", "");
                set_to = string::replace(set_to, "]", "");
                set_to = string::replace(set_to, "\"", "");
                if (std::string(set_to).empty()) set_to = fmt::format(
                    "{} [{}]",
                    getJsonData(issue)["user"]["login"].as_string(),
                    getJsonData(issue)["user"]["id"].as_int()
                ).data();
                set_to = ("By: " + std::string(set_to));
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*desc*/ {
                auto val = "desc";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "desc", "no desc");
                if (mod.contains("description")) set_to = mod["description"].as_string();
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            if (getJsonData(asset).contains("size")) {
                auto val = "size";
                auto set_to = getJsonData(asset)["size"].as_int();
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            if (getJsonData(asset).contains("download_count")) {
                auto val = "downloads";
                auto set_to = getJsonData(asset)["download_count"].as_int();
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*body_base64*/ {
                auto val = "body_base64";
                std::string set_to = fmt::format(
                    "# {}\n{}",
                    json["title"].as_string(),
                    json["desc"].as_string()
                );
                if (not string::contains(readme, "404: Not Found"))
                    set_to = readme;
                if (not string::contains(about, "404: Not Found"))
                    set_to = about;
                set_to = ZipUtils::base64URLEncode(set_to);
                json[val] = matjson::Value(std::string(set_to.data()).c_str());
                log(fmt::format("{} = {}", val, set_to));
            }
            /*issue_json_base64*/ {
                auto val = "issue_json_base64";
                std::string set_to = getData(data::issue);
                set_to = ZipUtils::base64URLEncode(set_to);
                json[val] = matjson::Value(std::string(set_to.data()).c_str());
                log(fmt::format("{} = {}", val, set_to));
            }
            /*download_link*/ {
                auto val = "download_link";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "download_link", "");
                if (std::string(set_to).empty()) {//so generate one
                    if (getIniData(data::issue_body_ini)->SectionExists("release")) {
                        auto file = getIniData(issue_body_ini)->GetValue("release", "file", "NO FILE NAME!!!");
                        auto tar = getIniData(issue_body_ini)->GetValue("release", "tar", "latest");
                        set_to = fmt::format("https://github.com/{}/{}/releases/{}/download/{}", repo_owner, repo_name, tar, file);
                    }
                }
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*download_path*/ {
                auto val = "download_path";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "download_path", "");
                set_to = string::replace(set_to, "..", "");
                if (std::string(set_to).empty()) {//so generate one
                    if (getIniData(data::issue_body_ini)->SectionExists("release")) {
                        auto file = getIniData(issue_body_ini)->GetValue("release", "file", "NO FILE NAME!!!");
                        auto geode = dirs::getGeodeDir().string();
                        if (string::contains(file, ".geode"))
                            set_to = geode + std::string("/mods/") + file;
                        if (string::containsAny(file, { ".dll", ".so" }))
                            set_to = geode + std::string("/ryzen/loadit/") + file;
                        if (string::containsAny(file, { ".zip" }))
                            set_to = geode + std::string("/config/geode.texture-loader/packs/") + file;
                    }
                    else {
                        auto link_path = ghc::filesystem::path(json["download_link"].as_string());
                        auto file = link_path.filename().string();
                        auto geode = dirs::getGeodeDir().string();
                        if (string::contains(file, ".geode"))
                            set_to = geode + std::string("/mods/") + file;
                        if (string::containsAny(file, { ".dll", ".so" }))
                            set_to = geode + std::string("/ryzen/loadit/") + file;
                        if (string::containsAny(file, { ".zip" }))
                            set_to = geode + std::string("/config/geode.texture-loader/packs/") + file;
                    }
                }
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*github_page_link*/ {
                auto val = "github_page_link";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", val, "");
                if (set_to.empty()) {
                    set_to = getJsonData(issue)["html_url"].as_string();
                    if (getJsonData(repo).contains("html_url")) 
                        set_to = getJsonData(repo)["html_url"].as_string();
                }
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            log(fmt::format("### main.json file: {}", working_dir("main.json").string()));
            log(json.dump());
            std::ofstream(working_dir("main.json")) << json.dump();
            //open view lay
            setViewLayerInIt(json);
        }
    }
    void load(matjson::Value IssueJson) {
        if (checkExistence(working_dir("main.json"))) return;
        CSimpleIni* main = new CSimpleIni;
        //issue_ini
        CSimpleIni issue_ini;
        issue_ini.LoadData(IssueJson["body"].as_string());
        //repo type
        if (issue_ini.SectionExists("repo")) {
            log("# repo type... getting repo now/");
            loadRepo();
        }
        else {
            log("# \"custom\" type... setting stuff now");
            //jsonsetup
            auto json = matjson::parse(
                "{                                  \
                    \"workindir\": \"\",            \
                    \"title\": \"\",                \
                    \"devs\": \"\",                 \
                    \"desc\": \"\",                 \
                    \"size\": 0,                    \
                    \"downloads\": 0,               \
                    \"body_base64\": \"\",          \
                    \"issue_json_base64\": \"\",    \
                    \"download_link\": \"\",        \
                    \"download_path\": \"\",        \
                    \"github_page_link\": \"\"      \
                }"
            );
            /*workindir*/ {
                auto val = "workindir";
                std::string set_to = working_dir().string();
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*title*/ {
                auto val = "title";
                std::string set_to = getJsonData(issue)["title"].as_string();
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*devs*/ {
                auto val = "devs";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "publisher", "");
                if (std::string(set_to).empty()) set_to = fmt::format(
                    "{} [{}]",
                    getJsonData(issue)["user"]["login"].as_string(),
                    getJsonData(issue)["user"]["id"].as_int()
                ).data();
                set_to = ("By: " + std::string(set_to));
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*desc*/ {
                auto val = "desc";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "desc", "no desc");
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*body_base64*/ {
                auto val = "body_base64";
                std::string set_to = fmt::format(
                    "# {}\n{}",
                    json["title"].as_string(),
                    json["desc"].as_string()
                );
                auto issue_body = std::string(getData(issue_body_ini));
                if (issue_body.find("body:") != std::string::npos) {
                    //pointas
                    auto start = issue_body.find("body:") + 7;
                    auto end = issue_body.find("^^^");
                    if(end == std::string::npos) end = issue_body.size();
                    //a
                    std::string substring = issue_body.substr(start, end - start + 1);
                    set_to = substring;
                }
                set_to = ZipUtils::base64URLEncode(set_to);
                json[val] = matjson::Value(std::string(set_to.data()).c_str());
                log(fmt::format("{} = {}", val, set_to));
            }
            /*issue_json_base64*/ {
                auto val = "issue_json_base64";
                std::string set_to = getData(data::issue);
                set_to = ZipUtils::base64URLEncode(set_to);
                json[val] = matjson::Value(std::string(set_to.data()).c_str());
                log(fmt::format("{} = {}", val, set_to));
            }
            /*download_link*/ {
                auto val = "download_link";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "download_link", "");
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*download_path*/ {
                auto val = "download_path";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "download_path", "");
                set_to = string::replace(set_to, "..", "");
                if (std::string(set_to).empty()) {//so generate one
                    auto download_link = json["download_link"].as_string();
                    if (not download_link.empty()) {
                        download_link = explode("?", download_link)[0];
                        auto filename = ghc::filesystem::path(download_link).filename().string();
                        auto geode = dirs::getGeodeDir().string();
                        if (string::contains(filename, ".geode"))
                            set_to = geode + std::string("/mods/") + filename;
                        if (string::containsAny(filename, { ".dll", ".so" }))
                            set_to = geode + std::string("/ryzen/loadit/") + filename;
                        if (string::containsAny(filename, { ".zip" }))
                            set_to = geode + std::string("/config/geode.texture-loader/packs/") + filename;
                    }
                }
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            /*github_page_link*/ {
                auto val = "github_page_link";
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", val, "");
                if (set_to.empty()) {
                    set_to = getJsonData(issue)["html_url"].as_string();
                }
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            log(fmt::format("### main.json file: {}", working_dir("main.json").string()));
            log(json.dump());
            std::ofstream(working_dir("main.json")) << json.dump();
            log("loading logo with ini main.logo_url");
            web::fetchFile(getIniData(issue_body_ini)->GetValue("main", "logo_url", ""), working_dir(".logo"));
            if(!CCSprite::create(working_dir(".logo").string().c_str()))
                web::fetchFile(getJsonData(data::issue)["user"]["avatar_url"].as_string(), working_dir(".logo"));
            //open view lay
            setViewLayerInIt(json);
        }
    }
    void log(std::string str = "", std::string endl = "\n\n") {
        log::debug("{}", str);
        auto pMDTextArea = dynamic_cast<MDTextArea*>(getChildByID("pMDTextArea"));
        pMDTextArea->setString((pMDTextArea->getString() + str + endl).data());
        pMDTextArea->getScrollLayer()->scrollLayer(pMDTextArea->getScrollLayer()->getContentHeight() * 2);
    }
    void setViewLayerInIt(matjson::Value data) {
        auto parent = this->getParent();
        removeFromParent();
        parent->addChild(ModViewLayer::create(data));
    }
    //other shit
    void onBtn(CCObject* pCCObject) {
        auto what = dynamic_cast<CCNode*>(pCCObject);
        if (not what) return;
        if (what->getID() == "back") keyBackClicked();
    }
    static void openMe(matjson::Value IssueJson) {
        auto scene = CCScene::create();
        auto a = ModLoadingLayer::create(IssueJson);
        if (checkExistence(a->working_dir("main.json"))) {
            ModViewLayer::openMe(
                matjson::parse(read_file(a->working_dir("main.json")))
            );
            return;
        }
        scene->addChild(a, 0, IssueJson["number"].as_int());
        CCDirector::sharedDirector()->pushScene(scene);
        a->load(IssueJson);
    };
    void keyBackClicked() {
        CCDirector::sharedDirector()->popScene();
    }
};

matjson::Value LAST_VIEWED_ISSUE;
class IssueItem : public CCMenuItem {
public:
    matjson::Value json;
    void onInfo(CCObject*) {
        auto asd = std::stringstream("asd");
        /**/asd << "```\n";
        asd << json.dump();
        asd << "\n```";
        auto pop = geode::MDPopup::create("Json data", asd.str(), "Yes");
        public_cast(pop, m_closeBtn)->setVisible(0);
        pop->show();
    }
    void onLabel(CCObject* sneder) {
        //node
        auto item = dynamic_cast<CCMenuItemSpriteExtra*>(sneder); if (!item) return;
        auto label = dynamic_cast<CCLabelTTF*>(item->getNormalImage()); if (!label) return;
        auto label_id = item->getID();
        //json
        auto label_json = matjson::Value();
        for (auto label : json["labels"].as_array()) {
            auto thatsit = string::contains(label["id"].dump(), label_id);
            if (thatsit) label_json = label;
            //log::debug("{}>contains>{}>{}.forA({})", label["id"].dump(), label_id, thatsit, label.dump());
        }
        if (not label_json.contains("id")) return;
        auto color = cocos::cc3bFromHexString(label_json["color"].as_string()).value_or(ccColor3B(190, 190, 200));
        if (label_json["description"].is_null()) 
            label_json["description"] = "seems to be created label by \n publisher via !setlabels comment";
        //pop
        auto pop = geode::createQuickPopup("", "\n \n \n ", "OK", nullptr, nullptr);
        auto desc_container = CCNode::create();
        pop->m_buttonMenu->addChild(desc_container);
        desc_container->setPositionY(82.f);
        //prev
        auto labelView = CCNode::create();
        labelView->addChild(label);
        labelView->setContentSize(label->getContentSize());
        labelView->setAnchorPoint(label->getAnchorPoint());
        labelView->setScale(1.9f);
        desc_container->addChild(labelView);
        //desc
        auto desc = CCLabelBMFont::create(label_json["description"].as_string().c_str(), "chatFont.fnt");
        if (label_json["description"].as_string().empty()) desc->setString("no description for that...");
        desc->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
        desc->setColor(color);
        desc_container->addChild(desc);
        //align items vertically
        reinterpret_cast<CCMenu*>(desc_container)->alignItemsVerticallyWithPadding(32.f);
        //dump
        auto dump = CCLabelTTF::create(label_json.dump().c_str(), "arial", 15.f);
        dump->setHorizontalAlignment(kCCTextAlignmentLeft);
        dump->setPositionY(-25.f);
        dump->setScale(0.64f);
        dump->setOpacity(6);
        dump->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
        desc_container->addChild(dump);
    }
    void view(CCObject*) {
        auto labels_str = json["labels"].dump();
        LAST_VIEWED_ISSUE = json;
        //CSimpleIni* TEMPONEEEA = new CSimpleIni;
        if (labels_str.find(fmt::format("\"{}\",", "mod")) != std::string::npos)
            return ModLoadingLayer::openMe(json);//ModViewLayer::openMe(TEMPONEEEA);
        if (labels_str.find(fmt::format("\"{}\",", "pack")) != std::string::npos) 
            return ModLoadingLayer::openMe(json);//ModViewLayer::openMe(TEMPONEEEA);
        auto pop = createQuickPopup(
            "No Labels!", 
            "This <cg>issue item</c> <cr>has not</c> <cb>\"mod\"</c> or <co>\"pack\"</c> label!",
            "Oh ok", nullptr, 
            360.f, nullptr, true
        );
    }
    static IssueItem* create(matjson::Value pJson, CCContentLayer* pContentLayer = nullptr, ScrollLayer* pScrollLayer = nullptr) {
        auto pRtn = new IssueItem();
        if (pRtn->init()) {
            //json
            pRtn->json = pJson;
            auto issue_publisher = fmt::format(
                "{} [{}]",
                pJson["user"]["login"].as_string(),
                pJson["user"]["id"].as_int()
            );
            auto issue_num = fmt::format("#{}", pJson["number"].as_int());
            //ini values
            auto issue_ini = new CSimpleIni();
            issue_ini->SetAllowKeyOnly(0);
            issue_ini->SetMultiLine(1);
            issue_ini->LoadData(pJson["body"].as_string());
            //sPublisher
            auto sPublisher = issue_ini->GetValue("main", "publisher", issue_publisher.data());
            sPublisher = std::string(sPublisher).empty() ? issue_publisher.data() : sPublisher;
            //sDesc
            auto sDesc = issue_ini->GetValue("main", "desc", "issue body ini hasn't desc value in main section");
            //sBottomRightCornerText
            auto sBottomRightCornerText = issue_ini->GetValue("main", "bottom_right_corner_text", issue_num.data());
            sBottomRightCornerText = std::string(sBottomRightCornerText).empty() ? issue_num.data() : sBottomRightCornerText;
            {
                //menu
                auto menu = CCMenu::create();
                menu->setPositionY(45.f);
                menu->setID("menu");
                pRtn->addChild(menu, 10);
                //square bg
                auto pCCLayerColor = CCLayerColor::create({ 0,0,0,75 });
                if (string::contains(pJson["labels"].dump(), "\"name\": \"verified\""))
                    pCCLayerColor->setColor({ 5, 25, 5 });
                if (string::contains(pJson["labels"].dump(), "\"name\": \"featured\""))
                    pCCLayerColor->setColor({ 25, 25, 5 });
                pCCLayerColor->setContentSize({ pScrollLayer->getContentWidth(), 60.f});//THE SIZE OF ITEM GOES HERE
                pCCLayerColor->setPosition(-(pCCLayerColor->getContentSize() / 2));
                menu->addChild(pCCLayerColor, -1);//add bg
                menu->setPosition(pCCLayerColor->getContentSize() / 2);//center the menu
                //fit up to scroll width
                auto SIZE = pCCLayerColor->getContentSize();
                auto POINTING_SIZE = pCCLayerColor->getContentSize() / 2;
                pRtn->setContentSize(SIZE);
                //Ryzen_InfoBtn_001
                CCMenuItemSpriteExtra* Ryzen_InfoBtn_001 = CCMenuItemSpriteExtra::create(
                    CCSprite::create("Ryzen_InfoBtn_001.png"_spr),
                    pRtn, menu_selector(IssueItem::onInfo)
                );
                Ryzen_InfoBtn_001->setPositionX(POINTING_SIZE.width - 8);
                Ryzen_InfoBtn_001->setPositionY(POINTING_SIZE.height - 8);
                menu->addChild(Ryzen_InfoBtn_001);
                /*ViewBtn*/ {
                    //text
                    CCLabelTTF* View = CCLabelTTF::create("   View   ", "Comic Sans MS.ttf"_spr, 12.f);
                    //btn
                    CCMenuItemSpriteExtra* ViewBtn = CCMenuItemSpriteExtra::create(
                        View, pRtn, menu_selector(IssueItem::view)
                    );
                    ViewBtn->setPositionX(POINTING_SIZE.width - 8);
                    ViewBtn->setAnchorPoint({ 1.0f, .5f });
                    ViewBtn->m_colorEnabled = true;
                    menu->addChild(ViewBtn);
                    //bg
                    auto grad = CCLayerGradient::create(
                        ccColor4B(10, 10, 10, 30),
                        ccColor4B(10, 10, 10, 130)
                    );
                    auto padding = 5.f;
                    grad->setContentSize(View->getContentSize() + CCPoint(padding, padding));
                    grad->setPosition(CCPoint(-padding, -padding) / 2);
                    View->addChild(grad, -1, 57290);
                }
                //id
                CCLabelTTF* CCLabelTTFid = CCLabelTTF::create(sBottomRightCornerText, "arial", 8.f);
                CCLabelTTFid->setOpacity(60);
                CCLabelTTFid->setHorizontalAlignment(kCCTextAlignmentRight);
                CCLabelTTFid->setAnchorPoint({ 1.0f, .0f });
                CCLabelTTFid->setPositionX(POINTING_SIZE.width - 4);
                CCLabelTTFid->setPositionY(3 - POINTING_SIZE.height);
                menu->addChild(CCLabelTTFid);
                //name and tags
                {
                    //container
                    auto container = CCMenu::create();
                    container->setAnchorPoint({ 0.0f, -0.8f });
                    container->setPositionX(10 - POINTING_SIZE.width);
                    container->setPositionY(.9f);
                    container->setContentWidth(SIZE.width - 90.f);
                    container->setLayout(
                        RowLayout::create()
                        ->setGap(5.f)
                        ->setAxisAlignment(AxisAlignment::Start)
                    );
                    menu->addChild(container);
                    //name
                    CCLabelTTF* name = CCLabelTTF::create(pJson["title"].as_string().c_str(), "arial", 10.f);
                    if (string::contains(pJson["labels"].dump(), "\"name\": \"verified\""))
                        name->setColor({ 190, 255, 190 });
                    if (string::contains(pJson["labels"].dump(), "\"name\": \"featured\""))
                        name->setColor({ 255, 255, 190 });
                    name->setHorizontalAlignment(kCCTextAlignmentLeft);
                    name->setAnchorPoint(CCPointZero);
                    container->addChild(name);
                    //tags
                    for (auto catgirl : pJson["labels"].as_array()) {
                        auto color = cocos::cc3bFromHexString(catgirl["color"].as_string()).value_or(ccColor3B(190, 190, 200));
                        auto lighter_color_dark_amount = 70;
                        auto lighter_color_boost = 60;
                        auto lighter_color = /*color;*/ ccColor3B(
                            color.r < lighter_color_dark_amount ? color.r + lighter_color_boost : color.r,
                            color.g < lighter_color_dark_amount ? color.g + lighter_color_boost : color.g,
                            color.b < lighter_color_dark_amount ? color.b + lighter_color_boost : color.b
                        );
                        //label
                        CCLabelTTF* tag = CCLabelTTF::create(catgirl["name"].as_string().c_str(), "arial", 8.f);
                        tag->setHorizontalAlignment(kCCTextAlignmentLeft);
                        tag->setAnchorPoint(CCPointZero);
                        tag->setColor(lighter_color);
                        tag->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
                        //item
                        auto item = CCMenuItemSpriteExtra::create(tag, pRtn, menu_selector(IssueItem::onLabel));
                        item->setID(catgirl["id"].dump());
                        container->addChild(item);
                        //bg
                        auto grad = CCLayerGradient::create(
                            ccColor4B(lighter_color.r, lighter_color.g, lighter_color.b, 60),
                            ccColor4B(lighter_color.r - 20, lighter_color.g - 20, lighter_color.b - 20, 10)
                        );
                        auto padding = 3.f;
                        grad->setContentSize(tag->getContentSize() + CCPoint(padding, padding));
                        grad->setPosition(CCPoint(-padding, -padding) / 2);
                        grad->setBlendFunc({ GL_SRC_ALPHA, GL_ONE });
                        tag->addChild(grad, -1, 57290);
                    }
                    //post update
                    container->updateLayout();
                };
                //desc
                TextArea* desc = TextArea::create(
                    fmt::format(
                        "By: {}\n{}",
                        sPublisher,
                        sDesc
                    ), "chatFont.fnt", 0.6f, 2000.f, { 0.0f, 1.0f }, 10.0f, false);
                desc->setPositionX(10 - POINTING_SIZE.width);
                desc->setAnchorPoint({ 0.0f, 0.6f });
                desc->setOpacity(180);
                /*fitlinesscale*/ {
                    auto max_width = (SIZE.width * 2) - 320.f;
                    auto node = desc->m_label;
                    for (int i = 0; i < node->getChildrenCount(); i++) {
                        auto inode = cocos::getChild(node, i);
                        if (inode) {
                            if (inode->getContentSize().width >= max_width) {
                                inode->setScale((max_width) / (inode->getContentSize().width / inode->getScale()));
                            }
                        }
                    }
                };
                menu->addChild(desc);
            }
            if (pContentLayer) pContentLayer->addChild(pRtn);
            return pRtn;
        }
        else {
            delete pRtn;
            pRtn = NULL;
            return NULL;
        }
    }
};
void openLastViewed() {
    ModLoadingLayer::openMe(LAST_VIEWED_ISSUE);
};

class IssuesListLayer : public CCLayer, DynamicScrollDelegate {
public:
    void sendBtnFunc(std::string id) {
        auto BtnCmd = CCNode::create();
        BtnCmd->setID(id);
        return this->onBtn(BtnCmd);
    }
    void onBtn(CCObject* pCCObject) {
        auto what = dynamic_cast<CCNode*>(pCCObject);
        if (not what) return;
        if (what->getID() == "back") keyBackClicked();
        if (what->getID() == "reload") downloadMods();
        if (what->getID() == "search")  {
            auto local_issues_file = ryzen_dir / "issues.json";
            auto local_issues = std::ifstream(local_issues_file);
            if (local_issues.is_open())
            {
                std::string line;
                std::stringstream stream;
                while (std::getline(local_issues, line)) {
                    stream << line;
                };
                setupMods(matjson::parse(stream.str()));
            }
            else downloadMods();
        };
        if (what->getID() == "add_link")  {
            web::openLinkInBrowser("https://github.com/user95401/Ryzen-Mods/issues/new/choose");
        };
    }
    std::vector<matjson::Value> filterOutMods(matjson::Value catgirls) {
        std::vector<matjson::Value> cutestcatgirls;
        for (auto catgirl : catgirls.as_array()) {
            bool skip = false;
            if (auto input = dynamic_cast<CCTextInputNode*>(this->getChildByIDRecursive("input"))) {
                auto filter_str = std::string(input->getString());
                //filters
                auto filters = explode("&", filter_str);
                for (auto filter : filters) {
                    //name
                    if (filter.find("name:") != std::string::npos) {
                        auto type = std::string("name:");
                        auto val = filter.replace(filter.find(type), type.size(), "");
                        auto at = catgirl["title"].as_string();
                        skip = at.find(val) == std::string::npos;
                    }
                    //by
                    else if (filter.find("by:") != std::string::npos) {
                        auto type = std::string("by:");
                        auto val = filter.replace(filter.find(type), type.size(), "");
                        auto at = catgirl["user"]["login"].as_string();
                        skip = at.find(val) == std::string::npos;
                    }
                    //labels
                    else if (filter.find("labels:") != std::string::npos) {
                        auto type = std::string("labels:");
                        auto val = filter.replace(filter.find(type), type.size(), "");
                        //all labels
                        auto at = std::stringstream();
                        for (auto label : catgirl["labels"].as_array())
                            at << label["name"].dump();
                        //asda
                        auto vflt_labels = explode(",", val);
                        bool founded = false;
                        for (auto label : vflt_labels) {
                            auto exact = "\"" + label + "\"";
                            founded = at.str().find(exact) != std::string::npos;
                            if (!founded) break;
                        }
                        skip = not founded;
                    }
                    //any
                    else if (not filter.empty()) {
                        skip = catgirl.dump().find(filter) == std::string::npos;
                    }
                }
            }
            if (not skip) cutestcatgirls.push_back(catgirl);
        }
        return cutestcatgirls;
    };
    void setupMods(matjson::Value catgirls) {
        if (not dynamic_cast<IssuesListLayer*>(this)) return;
        auto scroll = dynamic_cast<ScrollLayer*>(this->getChildByID("scroll"));
        if (not scroll) return;
        scroll->m_contentLayer->removeAllChildren();
        scroll->m_contentLayer->setContentHeight(0.f);
        auto index = 0;
        auto cutestcatgirls = filterOutMods(catgirls);
        for (auto catgirl : cutestcatgirls) {
            //item add
            auto item = IssueItem::create(catgirl, scroll->m_contentLayer, scroll);
            //long down
            scroll->m_contentLayer->setContentHeight(//make content layer longer
                scroll->m_contentLayer->getContentHeight() + item->getContentHeight()
            );
            //add border if here next item will
            if (cutestcatgirls.size() > index + 1) {
                CCScale9Sprite* border = CCScale9Sprite::createWithSpriteFrameName("floorLine_01_001.png");
                scroll->m_contentLayer->addChild(border);
                border->setContentWidth(scroll->getContentWidth());
                //long down
                scroll->m_contentLayer->setContentHeight(//make content layer longer
                    scroll->m_contentLayer->getContentHeight() + border->getContentHeight()
                );
            }
            ++index;
        }
        //fix some shit goes when content smaller than scroll
        if (scroll->m_contentLayer->getContentSize().height < scroll->getContentSize().height) {
            scroll->m_contentLayer->setContentSize({
                scroll->m_contentLayer->getContentSize().width,
                scroll->getContentSize().height
                });
        }
        //upd
        scroll->m_contentLayer->updateLayout();
        //scroll->m_contentLayer->setContentWidth(scroll->getContentWidth());
        scroll->moveToTop();
    }
    void downloadMods() {
        auto downloading_ntfy = geode::Notification::create("Downloading mods...", NotificationIcon::Loading, 15.f); 
        downloading_ntfy->show();
        web::AsyncWebRequest()
            ghapiauth
            .fetch("https://api.github.com/repos/user95401/Ryzen-Mods/issues")
            .json()
            .then(
                [this, downloading_ntfy](matjson::Value const& catgirls) {
                    // do something with the catgirls :3
                    setupMods(catgirls);
                    auto local_issues_save = ryzen_dir / "issues.json";
                    ghc::filesystem::create_directories(local_issues_save.parent_path());
                    std::ofstream(local_issues_save.string().c_str()) << catgirls.dump();
                    downloading_ntfy->setString("Mods downloaded!");
                    downloading_ntfy->setIcon(NotificationIcon::Success);
                    downloading_ntfy->setTime(1.0f);/**/
                }
            )
            .expect(
                [this, downloading_ntfy](std::string const& error)
                {// something went wrong with our web request Q~Q
                    downloading_ntfy->hide();
                    auto message = error;
                    std::string* gofuckyourself;// = std::string("hatethisok");
                    auto json = matjson::parse(message, *gofuckyourself);
                    if (json.has_value() and json.value().contains("message")) {
                        message = json.value()["message"].as_string();
                    }
                    auto asd = geode::createQuickPopup(
                        "Request exception",
                        message,
                        "Nah", nullptr, 420.f, nullptr, false
                    );
                    asd->m_scene = this;
                    asd->show();
                }
           );
    }
    static IssuesListLayer* create(std::string withFilter = "") {
        auto rtn = new IssuesListLayer();
        if (rtn) {
            rtn->autorelease();
            rtn->init();
            basicRznLayersInit(rtn, menu_selector(IssuesListLayer::onBtn));
            GitHubAuthPopup::addMyBtn(rtn);
            GameManager::sharedState()->fadeInMusic(
                "Fishbone.mp3"
                ""_spr
            );
            {
                /*toprightbar*/ {
                    auto menu = CCMenu::create();
                    rtn->addChild(menu);
                    //reloadmodsbtn
                    CCMenuItemSpriteExtra* reload = CCMenuItemSpriteExtra::create(
                        CCSprite::create("Ryzen_ReloadBtn_001.png"_spr),
                        rtn, menu_selector(IssuesListLayer::onBtn)
                    );
                    reload->setID("reload");
                    reload->setPosition({
                        (CCDirector::sharedDirector()->getWinSize().width / 2) - (reload->getContentWidth() / 2),
                        (CCDirector::sharedDirector()->getWinSize().height / -2) + (reload->getContentHeight() / 2)
                        });
                    reload->getNormalImage()->setScale(0.7f);
                    menu->addChild(reload);//add GJ_replayBtn_001
                    //gj_findBtn
                    auto gj_findBtn = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"),
                        rtn, menu_selector(IssuesListLayer::onBtn)
                    );
                    gj_findBtn->setID("search");
                    gj_findBtn->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 82.000f });
                    menu->addChild(gj_findBtn);//add GJ_replayBtn_001
                    //addmodbtn
                    CCMenuItemSpriteExtra* GJ_plus3Btn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png"),
                        rtn, menu_selector(IssuesListLayer::onBtn)
                    );
                    GJ_plus3Btn_001->setID("add_link");
                    GJ_plus3Btn_001->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 46.000f });
                    GJ_plus3Btn_001->getNormalImage()->setScale(1.700f);
                    menu->addChild(GJ_plus3Btn_001);//add GJ_replayBtn_001
                };
                //scroll
                auto paddingx = 146.f;
                auto paddingt = 36.f;
                auto scroll_size = CCSize(CCDirector::get()->getScreenRight() - paddingx - 4, CCDirector::get()->getScreenTop() - paddingt);
                auto scroll = geode::ScrollLayer::create(scroll_size);
                {
                    scroll->setID("scroll");
                    scroll->m_contentLayer->setLayout(
                        ColumnLayout::create()
                        ->setGap(0.f)
                        ->setAutoScale(false)
                        ->setAxisReverse(true)
                        ->setAxisAlignment(AxisAlignment::End)
                    );
                    scroll->setPositionX(paddingx / 2 + 2);
                    rtn->addChild(scroll);
                };
                //top header idk
                {
                    CCScale9Sprite* header = CCScale9Sprite::createWithSpriteFrameName("playerSquare_001.png");
                    header->setContentSize({
                        scroll_size.width + 3, // for right left borders
                        paddingt + 1 // transept border away 
                    });
                    header->setAnchorPoint({ 0.5f, 1.0f });
                    header->setColor(ccBLACK);
                    header->setPositionX(CCDirector::get()->getScreenRight() / 2);
                    header->setPositionY(CCDirector::get()->getScreenTop());
                    rtn->addChild(header);
                    //sick grradient shit init
                    {
                        auto gradpadding = 3.f;
                        auto grad = CCLayerGradient::create(
                            ccColor4B(90, 90, 100, 255),
                            ccColor4B(70, 70, 72, 255)
                        );
                        grad->setPosition(CCPoint(gradpadding, gradpadding) / 2);
                        grad->setContentSize(header->getContentSize() - gradpadding);
                        header->addChild(grad);
                    }
                    //rlborders
                    {
                        CCScale9Sprite* l = CCScale9Sprite::createWithSpriteFrameName("edit_vLine_001.png");
                        header->addChild(l);
                        l->setPosition(
                            0,
                            header->getContentSize().height
                        );
                        l->setColor(ccBLACK);
                        l->setScaleX(1.5f);
                        l->setAnchorPoint({ 0.0, 0.5f });
                        l->setScaleY((CCDirector::get()->getScreenTop() * 2) / l->getContentSize().height);
                        CCScale9Sprite* r = CCScale9Sprite::createWithSpriteFrameName("edit_vLine_001.png");
                        header->addChild(r);
                        r->setPosition(
                            header->getContentSize().width,
                            header->getContentSize().height
                        );
                        r->setColor(ccBLACK);
                        r->setScaleX(1.5f);
                        r->setAnchorPoint({ 1.0, 0.5f });
                        r->setScaleY((CCDirector::get()->getScreenTop() * 2) / r->getContentSize().height);
                    }
                    //not actually header child ever
                    {
                        auto input = CCTextInputNode::create(
                            header->getContentSize().width - 10,
                            header->getContentSize().height,
                            "Filters...",
                            "chatFont.fnt"
                        );
                        input->setString(withFilter);
                        input->m_filterSwearWords = false;
                        input->m_allowedChars = " !\"#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
                        input->setAllowedChars(input->m_allowedChars);
                        input->setID("input");
                        input->m_placeholderColor = ccColor3B(160, 160, 160);
                        input->m_placeholderLabel->setColor(input->m_placeholderColor);
                        input->setPosition(header->getPosition());
                        input->setPositionY(input->getPositionY() - (header->getContentSize().height / 2));
                        rtn->addChild(input);
                    }
                }
            };
            rtn->sendBtnFunc("search");
        }
        else {
            delete rtn;
            rtn = nullptr;
        }
        return rtn;
    }
    void openMe(cocos2d::CCObject* object) {
        auto scene = CCScene::create();
        auto pRyzenLayer = IssuesListLayer::create("labels:mod&");
        scene->addChild(pRyzenLayer, 1, 2816);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    };
    void openMeForPacks(cocos2d::CCObject* object) {
        auto scene = CCScene::create();
        auto pRyzenLayer = IssuesListLayer::create("labels:pack&");
        scene->addChild(pRyzenLayer, 1, 2816);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    };
    void keyBackClicked() {
        GameManager::sharedState()->fadeInMenuMusic();
        CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }
};

void ModListLayer::tryCustomSetup(float) {
    if (!this) return;
    lastCreatedOne = this;
    //if (this->m_menu) this->m_menu->setRotation(12.f);
    //RyzenLayerBtn
    auto RyzenLayerBtn = CCMenuItemSpriteExtra::create(
        CCSprite::create("Ryzen_LogoBtn_001.png"_spr),
        this, menu_selector(IssuesListLayer::openMe)
    );
    RyzenLayerBtn->getNormalImage()->setScale(0.6f);
    RyzenLayerBtn->setPosition(-210.f, -33.f);
    this->m_topMenu->addChild(RyzenLayerBtn, 999, 5819);
}
void PackSelectLayer::tryCustomSetup(float) {
    if (!this) return;
    lastCreatedOne = this;
    auto menu = cocos::getChildOfType<CCMenu>(this, 0);
    //back btn
    auto backbtn = getChild<CCMenuItemSpriteExtra>(menu, 0);
    backbtn->setTarget(this, menu_selector(PackSelectLayer::back));
    //button
    auto text = CCLabelTTF::create("Search for TPs!", "Comic Sans MS.ttf"_spr, 15.f);
    //item
    auto RyzenLayerBtn = CCMenuItemSpriteExtra::create(
        text,
        this,
        menu_selector(IssuesListLayer::openMeForPacks)
    );
    RyzenLayerBtn->m_animationEnabled = 0;
    RyzenLayerBtn->m_colorEnabled = 1;
    RyzenLayerBtn->setPositionY(110.f);
    menu->addChild(RyzenLayerBtn);
}

#include <Geode/modify/CCLayer.hpp>
class $modify(CCLayerExt, CCLayer) {
    bool init() {
        auto rtn = CCLayer::init();
        //ModListLayer
        ModListLayer* pModListLayer = typeinfo_cast<ModListLayer*>(this);
        if (pModListLayer) pModListLayer->scheduleOnce(schedule_selector(ModListLayer::tryCustomSetup), 0.001f);
        //PackSelectLayer
        PackSelectLayer* pPackSelectLayer = typeinfo_cast<PackSelectLayer*>(this);
        if (pPackSelectLayer) pPackSelectLayer->scheduleOnce(schedule_selector(PackSelectLayer::tryCustomSetup), 0.001f);
        return rtn;
    };
};

#if defined(GEODE_IS_ANDROID)
#include <android/log.h>
#include <dlfcn.h>
#include <jni.h>
#include <unistd.h>
#endif
void loadMods() {
    auto path = ryzen_dir / "loadit";
    ghc::filesystem::create_directories(path);
    for (const auto& entry : ghc::filesystem::directory_iterator(path)) {
        auto filename = entry.path().filename();
        bool loadrtn = false;
        //loadit
#if defined(GEODE_IS_WINDOWS)
        if(filename.extension() == ".dll") {
            loadrtn = LoadLibrary(entry.path().string().data());
        }
#elif defined(GEODE_IS_ANDROID)
        if (filename.extension() == ".so") {
            //fuck
            char path[64] = {0};
            sprintf(path, "/proc/%d/cmdline", getpid());
            FILE* cmdline = fopen(path, "r");

            char application_id[64] = {0};
            fread(application_id, sizeof(application_id), 1, cmdline);
            fclose(cmdline);

            std::string gamePath = "/data/data/" + std::string(application_id);

            ghc::filesystem::copy_file(
                entry.path(), 
                gamePath / entry.path().filename(),
                ghc::filesystem::copy_options::update_existing
            );
            
            loadrtn = dlopen(
                (gamePath / entry.path().filename()).string().data(),
                RTLD_LAZY
            );
        }   
#endif
        auto log = fmt::format(
            "the {} {}",
            filename.string(),
            (not loadrtn ? "is failed to load" : "is loaded")
        );
        if (loadrtn) log::info("{}", log);
        else log::error("{}", log);
    }
}
$execute{ loadMods(); }
