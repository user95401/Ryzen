#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/Utils.hpp>
#include "ghapiauth.h"
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

auto basicRznLayersInit(CCLayer* rtn, cocos2d::SEL_MenuHandler onBtnSel) {
    {
        //setup
        rtn->setKeypadEnabled(true);
        rtn->setTouchEnabled(true);
        CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
        backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
        backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
        backgroundSprite->setAnchorPoint({ 0, 0 });
        backgroundSprite->setColor({ 90, 90, 100 });
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
void openModView(matjson::Value json);
void openModLoading(matjson::Value IssueJson);

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
        /*top center card*/ {
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
                auto logo = CCSprite::create((ghc::filesystem::path(strKeyOfdata("workindir")) / "logo.png").string().c_str());
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
                    auto size = CCSize(186.f, 68.f);
                    auto parent = text_part_container;
                    parent->setID("text_part_container");
                    parent->setContentSize(size);
                    parent->setLayout(
                        ColumnLayout::create()
                        ->setCrossAxisLineAlignment(AxisAlignment::Start)
                        ->setAxisAlignment(AxisAlignment::Even)
                        ->setAxisReverse(true)
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
                        downloadBtn->setScale(0.8f);
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
        /* tr buttons */ {
            CCMenu* menu = CCMenu::create();
            if (menu) {
                menu->setPosition(CCDirector::get()->getScreenRight(), 0.f);
                menu->setAnchorPoint({ 1.f, 0.f });
                menu->setLayout(
                    ColumnLayout::create()
                    ->setAxisAlignment(AxisAlignment::Start)
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
            //info
            if (auto sprite = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png")) {
                CCMenuItemSpriteExtra* info = CCMenuItemSpriteExtra::create(
                    sprite, this, menu_selector(ModViewLayer::onBtn)
                );
                info->setID("info");
                //reload->getNormalImage()->setScale(0.7f);
                menu->addChild(info);
            };
            menu->updateLayout();
        }
        /*md*/ {
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
        auto json_container = dynamic_cast<CCLabelBMFont*>(getChildByID("json_container"));//ini_container
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
        auto what = dynamic_cast<CCNode*>(pCCObject);
        if (not what) return;
        if (what->getID() == "reload") {
            if (checkExistence(ghc::filesystem::path(strKeyOfdata("workindir")) / "main.json"))
                remove_dir(ghc::filesystem::path(strKeyOfdata("workindir")));
            auto issue_json_text = ZipUtils::base64URLDecode(strKeyOfdata("issue_json_base64"));
            log::debug("{}", issue_json_text);
            openModLoading(matjson::parse(issue_json_text));
        };
        if (what->getID() == "back") keyBackClicked();
        if (what->getID() == "download") {
            //endpoint
            auto endpoint = std::string(strKeyOfdata("download_link"));
            auto path = ghc::filesystem::path(strKeyOfdata("download_path"));
            auto pop = geode::MDPopup::create(
                "Download mod?",
                "\n# From:\n" + endpoint + ""
                "\n# To:\n<cj>" + path.string(),
                "Abort", "Start",
                [this, endpoint, path, what](bool btn2) {
                    if (not btn2) return;
                    auto dwnloading_overlap = Notification::create("Downloading...", NotificationIcon::Loading, 0.f);
                    dwnloading_overlap->setPosition(what->getContentSize() / 2);
                    dwnloading_overlap->setScale(0.6f);
                    what->addChild(dwnloading_overlap);
                    web::AsyncWebRequest()ghapiauth.fetch(endpoint).into(path)
                        .then(
                            [this, dwnloading_overlap](std::monostate const& who) {
                                dwnloading_overlap->removeFromParent();
                                auto asd = geode::createQuickPopup(
                                    "Restart Game?",
                                    "To load new mod",
                                    "Later", "Yes",
                                    [](auto, bool btn2) {
                                        if (btn2) utils::game::restart();
                                    },
                                    false
                                );
                                asd->m_scene = this;
                                asd->show();
                            })
                        //utils::MiniFunction<void(SentAsyncWebRequest&, double, double)>;
                                .progress(
                                    [dwnloading_overlap](auto, double d1, double d2) {
                                        dwnloading_overlap->setString(
                                            fmt::format(
                                                "{} / {}",
                                                abbreviateNumber(d1),
                                                abbreviateNumber(d2)
                                            ).c_str());
                                    })
                                .expect(
                                    [this, endpoint, dwnloading_overlap](std::string const& what) {
                                        dwnloading_overlap->setIcon(NotificationIcon::Error);
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
            pop->show();
        };
    }
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
                (dirs::getGeodeDir() / "ryzen" / "mods" /
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
        //letsgo
        rtn->load(IssueJson);
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
                "https://raw.githubusercontent.com/{}/{}/{}/logo.png"
                , repo_owner, repo_name, default_branch
            );
            if (not owerwrite_endpoint.empty()) file_url = owerwrite_endpoint;
            log(fmt::format("file_url = {}", file_url.data()));
            //expect
            auto expect =
                [this, file_url](std::string const& what) {
                log("## <cr>" + what + "</c>");
                if (string::contains(file_url, "logo.png"))
                    loadRepo(2, string::replace(file_url, "logo.png", "pack.png"));
                if (string::contains(file_url, "pack.png"))
                    loadRepo(2, getJsonData(data::repo)["owner"]["avatar_url"].as_string());
                };
            //then
            auto then =
                [this, expect](std::monostate const& a) {
                if (not CCSprite::create(working_dir("logo.png").string().data())) expect("Bad image downloaded");
                log("```\nloaded :D\n```");
                loadRepo(3);
                };
            web::AsyncWebRequest()ghapiauth.fetch(file_url).into(working_dir("logo.png")).then(then).expect(expect);
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
                    \"download_path\": \"\"         \
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
                std::string set_to = getIniData(issue_body_ini)->GetValue("main", "desc", "");
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
                std::string set_to = json["desc"].as_string();
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
                    }
                }
                json[val] = (set_to);
                log(fmt::format("{} = {}", val, set_to));
            }
            log(fmt::format("{}", working_dir("main.json").string()));
            log(json.dump());
            std::ofstream(working_dir("main.json")) << json.dump();
            ModViewLayer::openMe(json);
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
            log("# \"custom\" type... srtting stuff now");
        }
    }
    void log(std::string str = "", std::string endl = "\n\n") {
        log::debug("{}", str);
        auto pMDTextArea = dynamic_cast<MDTextArea*>(getChildByID("pMDTextArea"));
        pMDTextArea->setString((pMDTextArea->getString() + str + endl).data());
        pMDTextArea->getScrollLayer()->scrollLayer(pMDTextArea->getScrollLayer()->getContentHeight() * 2);
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
    };
    void keyBackClicked() {
        CCDirector::sharedDirector()->popScene();
    }
};

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
        if (label_json["description"].is_null()) label_json["description"] = "seems to be created label by \npublisher via !setlabels comment";
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
        auto desc = CCLabelTTF::create(label_json["description"].as_string().c_str(), "arial", 15.f);
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
        if (what->getID() == "search") {
            auto local_issues_file = dirs::getGeodeDir() / "ryzen" / "issues.json";
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
        if (what->getID() == "add_link") web::openLinkInBrowser("https://github.com/user95401/Ryzen-Mods/issues/new/choose");
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
                    auto local_issues_save = dirs::getGeodeDir() / "ryzen" / "issues.json";
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
            GameManager::sharedState()->fadeInMusic(
                "WhiteNoiseBlackVoid - Night Walk Through the Grayland.mp3"
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
                auto paddingy = 36.f;
                auto scroll_size = CCSize(CCDirector::get()->getScreenRight() - paddingx - 4, CCDirector::get()->getScreenTop() - paddingy);
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
                        paddingy + 1 // transept border away 
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
    void tryCustomSetup(float) {
        if (!this) return;
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
};
class PackSelectLayer : public CCLayer {
public:
    ScrollLayer* m_availableList = nullptr;
    ScrollLayer* m_appliedList = nullptr;
    void tryCustomSetup(float) {
        if (!this) return;
        auto menu = cocos::getChildOfType<CCMenu>(this, 0);
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
};
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

void openModView(matjson::Value data) {
    ModViewLayer::openMe(data);
};
void openModLoading(matjson::Value IssueJson) {
    ModLoadingLayer::openMe(IssueJson);
};

#if defined(GEODE_IS_ANDROID)
#include <android/log.h>
#include <dlfcn.h>
#include <jni.h>
#include <unistd.h>
#endif

void loadMods() {
    auto path = dirs::getGeodeDir() / "ryzen" / "loadit";
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
            loadrtn = true;
            dlopen(entry.path().string().data(), RTLD_LAZY);
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
