#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "ghapiauth.h"
using namespace geode::prelude;

template <typename T>
bool checkExistence(T filename)
{
    std::ifstream Infield(filename);
    return Infield.good();
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

auto basicRznLayersInit(CCLayer* rtn, cocos2d::SEL_MenuHandler onBtnSel) {
    {
        //setup
        rtn->setKeypadEnabled(true);
        rtn->setTouchEnabled(true);
        /*bg*/ {
            CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
            backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
            backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
            backgroundSprite->setAnchorPoint({ 0, 0 });
            backgroundSprite->setColor({ 70, 80, 90 });
            backgroundSprite->runAction(CCRepeatForever::create(
                CCSequence::create(
                    CCTintTo::create(5.0, 100, 100, 100),
                    CCTintTo::create(5.0, 100, 100, 110),
                    CCTintTo::create(5.0, 105, 100, 110),
                    nullptr
                )
            ));
            rtn->addChild(backgroundSprite, -2);
        };
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

#include "SimpleIni.h"
class ModViewLayer : public CCLayer {
public:
    enum ModType { Undef = 1510, Mod = 1511, Pack = 1512 };
    static auto create(matjson::Value pJson) {
        auto rtn = new ModViewLayer;
        rtn->init();
        basicRznLayersInit(rtn, menu_selector(ModViewLayer::onBtn));
        /* text containers 💀 */ {
            //issueJson
            auto issueJson = CCLabelTTF::create(pJson.dump().data(), "arial", 6.f);
            issueJson->setVisible(0);
            issueJson->setID("issueJson");
            rtn->addChild(issueJson, 999);
            //repoJson
            auto repoJson = CCLabelTTF::create("{}", "arial", 6.f);
            repoJson->setVisible(0);
            repoJson->setID("repoJson");
            rtn->addChild(repoJson, 999);
            //metaJson
            auto metaJson = CCLabelTTF::create("{}", "arial", 6.f);
            metaJson->setVisible(0);
            metaJson->setID("metaJson");
            rtn->addChild(metaJson, 999);
            //releaseJson
            auto releaseJson = CCLabelTTF::create("{}", "arial", 6.f);
            releaseJson->setVisible(0);
            releaseJson->setID("releaseJson");
            rtn->addChild(releaseJson, 999);
            //ini
            auto ini = CCLabelTTF::create(pJson["body"].as_string().data(), "arial", 6.f);
            ini->setVisible(0);
            ini->setID("ini");
            rtn->addChild(ini, 999);
            //detect type
            auto labels_str = pJson["labels"].dump();
            ModType type = ModType::Undef;
            if (labels_str.find(fmt::format("\"{}\",", "mod")) != std::string::npos) type = ModType::Mod;
            if (labels_str.find(fmt::format("\"{}\",", "pack")) != std::string::npos) type = ModType::Pack;
            auto typeMark = CCLabelTTF::create("Undef:1510,Mod:1511,Pack:1512", "arial", 6.f);
            typeMark->setID("type");
            typeMark->setVisible(0);
            rtn->addChild(typeMark, 999, type);
            //path
            auto the_path = dirs::getGeodeDir() / "ryzen" / "mods" / fmt::to_string(pJson["number"]);
            ghc::filesystem::create_directories(the_path);
            auto path = CCLabelTTF::create(the_path.string().data(), "arial", 6.f);
            path->setVisible(0);
            path->setID("path");
            rtn->addChild(path, 999);
        }
        /* bottom_inf_text */ {
            auto inf_label = CCLabelTTF::create(
                fmt::format(
                    "repo: {}, release: {}",
                    rtn->ini()->GetValue("mod", "repo"),
                    rtn->ini()->GetValue("mod", "release_tag")
                ).data(),
                "arial", 8.f
            );
            inf_label->setID("inf_label");
            inf_label->setOpacity(120);
            inf_label->setAnchorPoint({ 0.5f, 0.f });
            inf_label->setPositionX(CCDirector::get()->getScreenRight() / 2);
            rtn->addChild(inf_label);
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
                rtn->addChild(menu);
            };
            //reloadmodsbtn
            if (auto sprite = CCSprite::create("Ryzen_ReloadBtn_001.png"_spr)) {
                CCMenuItemSpriteExtra* reload = CCMenuItemSpriteExtra::create(
                    sprite, rtn, menu_selector(ModViewLayer::onBtn)
                );
                reload->setID("reload");
                reload->getNormalImage()->setScale(0.7f);
                menu->addChild(reload);//add GJ_replayBtn_001
            };
            menu->updateLayout();
        }
        rtn->loadDataMain();
        return rtn;
    }
    void loadDataMain() {
        //prepare ntfy and other ui mayb
        auto loadings = CCMenu::create();
        loadings->setID("loadings");
        addChild(loadings);
        auto loading_repo = Notification::create("Loading repo...", NotificationIcon::Loading, 0.f);
        loading_repo->setID("loading_repo");
        loadings->addChild(loading_repo);
        auto loading_logo = Notification::create("Loading logo...", NotificationIcon::Loading, 0.f);
        loading_logo->setID("loading_logo");
        loadings->addChild(loading_logo);
        auto loading_meta = Notification::create("Loading meta...", NotificationIcon::Loading, 0.f);
        loading_meta->setID("loading_meta");
        loadings->addChild(loading_meta);
        auto loading_release = Notification::create("Loading release...", NotificationIcon::Loading, 0.f);
        loading_release->setID("loading_release");
        loadings->addChild(loading_release);
        loadings->alignItemsVerticallyWithPadding(32.f);
        //letsgo
        loadRepo();
    }
    void loadStep2() {
        loadLogo();
        loadMeta();
        loadRelease();
        waitForCustomSetup();
    }
    //loaders
    void loadRepo() {
        //vars prepare
        auto loading_repo = dynamic_cast<Notification*>(getChildByIDRecursive("loading_repo"));
        auto repoJson = dynamic_cast<CCLabelTTF*>(getChildByIDRecursive("repoJson"));
        auto endpoint = fmt::format("https://api.github.com/repos/{}", ini()->GetValue("mod", "repo"));
        auto file = workindir() / "repo.json";
        //req
        if (checkExistence(file)) {
            auto filestream = std::ifstream(file);
            if (filestream.is_open())
            {
                std::stringstream stream;
                {
                    std::string line;
                    while (std::getline(filestream, line)) {
                        stream << line;
                    };
                };
                repoJson->setString(stream.str().data());
                loading_repo->setString("Repository loaded from local");
                loading_repo->setIcon(NotificationIcon::Success);
                loading_repo->setTag(1);
                loadStep2();
            }
        }
        else {
            web::AsyncWebRequest()ghapiauth.fetch(endpoint).json()
                .then(
                    [this, loading_repo, file, repoJson](matjson::Value const& catgirls) {
                        repoJson->setString(catgirls.dump().data());
                        std::ofstream(file.string().c_str()) << catgirls.dump();
                        loading_repo->setString("Repository loaded");
                        loading_repo->setIcon(NotificationIcon::Success);
                        loading_repo->setTag(1);
                        loadStep2();
                    })
                .expect(
                    [this, loading_repo, endpoint](std::string const& what) {
                        loading_repo->setIcon(NotificationIcon::Error);
                        auto asd = geode::createQuickPopup(
                            "Request exception",
                            what + "\n" + endpoint,
                            "Nah", nullptr, 420.f, nullptr, false
                        );
                        asd->m_scene = this;
                        asd->show();
                    });
        };
    }
    void loadRelease() {
        //vars prepare
        auto loading_release = dynamic_cast<Notification*>(getChildByIDRecursive("loading_release"));
        auto releaseJson = dynamic_cast<CCLabelTTF*>(getChildByIDRecursive("releaseJson"));
        auto endpoint = fmt::format(
            "https://api.github.com/repos/{}/releases/{}",
            ini()->GetValue("mod", "repo"), ini()->GetValue("mod", "release_tag")
        );
        auto file = workindir() / "release.json";
        //req
        if (checkExistence(file)) {
            auto filestream = std::ifstream(file);
            if (filestream.is_open())
            {
                std::stringstream stream;
                {
                    std::string line;
                    while (std::getline(filestream, line)) {
                        stream << line;
                    };
                };
                releaseJson->setString(stream.str().data());
                loading_release->setString("Repository loaded from local");
                loading_release->setIcon(NotificationIcon::Success);
                loading_release->setTag(1);
            }
        }
        else {
            web::AsyncWebRequest()ghapiauth.fetch(endpoint).json()
                .then(
                    [this, loading_release, file, releaseJson](matjson::Value const& catgirls) {
                        releaseJson->setString(catgirls.dump().data());
                        std::ofstream(file.string().c_str()) << catgirls.dump();
                        loading_release->setString("Repository loaded");
                        loading_release->setIcon(NotificationIcon::Success);
                        loading_release->setTag(1);
                    })
                .expect(
                    [this, loading_release, endpoint](std::string const& what) {
                        loading_release->setIcon(NotificationIcon::Error);
                        auto asd = geode::createQuickPopup(
                            "Request exception",
                            what + "\n" + endpoint,
                            "Nah", nullptr, 420.f, nullptr, false
                        );
                        asd->m_scene = this;
                        asd->show();
                    });
        };
    }
    void loadMeta() {
        //vars prepare
        auto loading_meta = dynamic_cast<Notification*>(getChildByIDRecursive("loading_meta"));
        auto metaJson = dynamic_cast<CCLabelTTF*>(getChildByIDRecursive("metaJson"));
        auto branch = repoJson()["default_branch"].as_string();
        if(std::string(ini()->GetValue("mod", "release_tag")) != "latest") branch = ini()->GetValue("mod", "release_tag");
        auto endpoint = fmt::format(
            "https://raw.githubusercontent.com/{}/{}/{}",
            ini()->GetValue("mod", "repo"),
            branch,
            type() == ModType::Mod ? "mod.json" : "pack.json"
        );
        auto file = workindir() / "meta.json";
        //req
        if (checkExistence(file)) {
            auto filestream = std::ifstream(file);
            if (filestream.is_open())
            {
                std::stringstream stream;
                {
                    std::string line;
                    while (std::getline(filestream, line)) {
                        stream << line;
                    };
                };
                metaJson->setString(stream.str().data());
                loading_meta->setString("Meta loaded from local");
                loading_meta->setIcon(NotificationIcon::Success);
                loading_meta->setTag(1);
            }
        }
        else {
            web::AsyncWebRequest()ghapiauth.fetch(endpoint).json()
                .then(
                    [this, loading_meta, file, metaJson](matjson::Value const& catgirls) {
                        metaJson->setString(catgirls.dump().data());
                        std::ofstream(file.string().c_str()) << catgirls.dump();
                        loading_meta->setString("Meta loaded");
                        loading_meta->setIcon(NotificationIcon::Success);
                        loading_meta->setTag(1);
                    })
                .expect(
                    [this, loading_meta, endpoint](std::string const& what) {
                        loading_meta->setIcon(NotificationIcon::Error);
                        auto asd = geode::createQuickPopup(
                            "Request exception",
                            what + "\n" + endpoint,
                            "Nah", nullptr, 420.f, nullptr, false
                        );
                        asd->m_scene = this;
                        asd->show();
                    });
        };
    }
    void loadLogo() {
        //vars prepare
        auto loading_logo = dynamic_cast<Notification*>(getChildByIDRecursive("loading_logo"));
        auto endpoint = fmt::format(
            "https://raw.githubusercontent.com/{}/{}/{}", 
            ini()->GetValue("mod", "repo"), 
            repoJson()["default_branch"].as_string(),
            type() == ModType::Mod ? "logo.png" : "pack.png"
        );
        auto file = workindir() / "icon.png";
        //req
        if (checkExistence(file)) {
            loading_logo->setString("Logo founded locally");
            loading_logo->setIcon(CCSprite::create(file.string().c_str()));
            loading_logo->setTag(1);
        }
        else {
            web::AsyncWebRequest()ghapiauth.fetch(endpoint).into(file)
                .then(
                    [this, loading_logo, file](std::monostate const& who) {
                        loading_logo->setString("Logo loaded");
                        loading_logo->setIcon(CCSprite::create(file.string().c_str()));
                        loading_logo->setTag(1);
                    })
                .expect(
                    [this, loading_logo, endpoint](std::string const& what) {
                        loading_logo->setIcon(NotificationIcon::Error);
                        loading_logo->setTag(1);
                        auto asd = geode::createQuickPopup(
                            "Request exception",
                            what + "\n" + endpoint,
                            "Nah", nullptr, 420.f, nullptr, false
                        );
                        asd->m_scene = this;
                        asd->show();
                    });
        };
    }
    void loadAnyReadme() {
        //about md
        {
            auto pMDTextArea = dynamic_cast<MDTextArea*>(getChildByIDRecursive("pMDTextArea"));
            //vars prepare
            auto endpoint = fmt::format(
                "https://raw.githubusercontent.com/{}/{}/{}",
                ini()->GetValue("mod", "repo"),
                repoJson()["default_branch"].as_string(),
                type() == ModType::Mod ? "about.md" : "README.md"
            );
            auto file = workindir() / "about.md";
            //req
            if (checkExistence(file)) {
                auto filestream = std::ifstream(file);
                if (filestream.is_open())
                {
                    std::stringstream stream;
                    {
                        std::string line;
                        while (std::getline(filestream, line)) {
                            stream << line;
                        };
                    };
                    pMDTextArea->setString(stream.str().c_str());
                }
            }
            web::AsyncWebRequest()ghapiauth.fetch(endpoint).text()
                .then(
                    [this, file, pMDTextArea](std::string const& catgirlwiki) {
                        std::ofstream(file.string().c_str()) << catgirlwiki;
                        pMDTextArea->setString(catgirlwiki.c_str());
                    })
                .expect(
                    [endpoint, pMDTextArea](std::string const& what) {
                        pMDTextArea->setString(("## <cr>" + what + "</c>\n" + endpoint).c_str());
                    });
        };
    }
    //customSetup
    void waitForCustomSetup(float asd = 1337.f) {
        auto sch = schedule_selector(ModViewLayer::waitForCustomSetup);
        if (asd == 1337.f) {
            this->schedule(sch, 0.1f);
        }
        else {
            auto loaded_repo = dynamic_cast<Notification*>(getChildByIDRecursive("loading_repo"))->getTag() == 1;
            auto loaded_logo = dynamic_cast<Notification*>(getChildByIDRecursive("loading_logo"))->getTag() == 1;
            auto loaded_meta = dynamic_cast<Notification*>(getChildByIDRecursive("loading_meta"))->getTag() == 1;
            auto loaded_release = dynamic_cast<Notification*>(getChildByIDRecursive("loading_release"))->getTag() == 1;
            if (loaded_repo and loaded_logo and loaded_meta and loaded_release) {
                dynamic_cast<CCMenu*>(getChildByIDRecursive("loadings"))->runAction(
                    CCEaseBackOut::create(CCMoveBy::create(0.5f, { 1000.f, 0.f }))
                );
                customSetup();
                this->unschedule(sch);
            }
        };
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
            );
            /*some fun stuff*/ {
                //icon
                auto icon = CCSprite::create((workindir() / "icon.png").string().c_str());
                if (!icon) icon = CCSprite::createWithSpriteFrameName("deleteFilter_none_001.png");
                if (icon) {
                    auto icon_size = CCSize(68.f, 68.f);
                    auto real_size = icon->getContentSize();
                    icon->setID("icon");
                    if (real_size.height > icon_size.height) {
                        icon->setScale(icon_size.height / real_size.height);
                    }
                    else {
                        icon->setScale(icon_size.width / real_size.width);
                    }
                    icon->setAnchorPoint(CCPointZero);
                    auto container = CCNode::create();
                    container->addChild(icon);
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
                            (metaJson()["name"].as_string() + " " + metaJson()["version"].as_string()).c_str(),
                            "arial", 18.f
                        );
                        parent->addChild(title);
                    }
                    /*devs*/ {
                        auto devs = std::stringstream() << "By: ";
                        if (metaJson().contains("developers")) {
                            for (auto dev : metaJson()["developers"].as_array()) {
                                devs << dev.as_string();
                            }
                        }
                        else devs << (type() == ModType::Mod ? metaJson()["developer"].as_string() : metaJson()["author"].as_string());
                        auto label = CCLabelTTF::create(devs.str().c_str(), "arial", 14.f);
                        parent->addChild(label);
                    }
                    /*description*/ {
                        auto strDesc = metaJson().contains("description") ?
                            metaJson()["description"].as_string().c_str() :
                            ini()->GetValue("mod", "desc", metaJson()["id"].as_string().c_str());
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
                    auto size = CCSize(186.f, 68.f);
                    auto parent = download_part;
                    parent->setID("download_part");
                    parent->setContentSize(size);
                    parent->setLayout(
                        ColumnLayout::create()
                        ->setCrossAxisLineAlignment(AxisAlignment::Start)
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
                    if (fileJson().contains("download_count")) {
                        CCMenu* statsContainerMenu = CCMenu::create();
                        parent->addChild(statsContainerMenu);
                        statsContainerMenu->setID("statsContainerMenu");
                        statsContainerMenu->setContentSize(size);
                        statsContainerMenu->setLayout(
                            RowLayout::create()
                            ->setGap(20.f)
                            ->setGrowCrossAxis(true)
                            ->setAxisAlignment(AxisAlignment::Start)
                        );
                        /*download_count*/ {
                            auto download_count = CCLabelTTF::create(
                                abbreviateNumber(fileJson()["download_count"].as_int()).c_str(),
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
                                convertSize(fileJson()["size"].as_int()).c_str(),
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
        }
        /*md*/ {
            auto pMDTextArea = MDTextArea::create(
                "# Content is loading...",
                { this->getContentWidth() - 120.f, this->getContentHeight() - 80.f, }
            );
            pMDTextArea->setID("pMDTextArea");
            pMDTextArea->setPositionX(this->getContentWidth() / 2);
            pMDTextArea->setAnchorPoint({ 0.5f, 0.f });
            addChild(pMDTextArea, -1, 85290);
            loadAnyReadme();
        };
    }
    //data gettinga
    ghc::filesystem::path workindir() {
        auto path = dynamic_cast<CCLabelTTF*>(this->getChildByIDRecursive("path"));
        return path->getString();
    }
    ModType type() {
        auto type = dynamic_cast<CCLabelTTF*>(this->getChildByIDRecursive("type"));
        return (ModType)type->getTag();
    }
    matjson::Value issueJson() {
        auto json = dynamic_cast<CCLabelTTF*>(this->getChildByIDRecursive("issueJson"));
        return matjson::parse(json->getString());
    }
    matjson::Value repoJson() {
        auto json = dynamic_cast<CCLabelTTF*>(this->getChildByIDRecursive("repoJson"));
        return matjson::parse(json->getString());
    }
    matjson::Value metaJson() {
        auto json = dynamic_cast<CCLabelTTF*>(this->getChildByIDRecursive("metaJson"));
        return matjson::parse(json->getString());
    }
    matjson::Value releaseJson() {
        auto json = dynamic_cast<CCLabelTTF*>(this->getChildByIDRecursive("releaseJson"));
        return matjson::parse(json->getString());
    }
    matjson::Value fileJson() {
        if (releaseJson()["assets"].as_array().empty()) return releaseJson();
        for (auto asset : releaseJson()["assets"].as_array()) {
            if (asset["name"].as_string() == ini()->GetValue("mod", "file")) {
                return asset;
            };
        }
        return releaseJson()["assets"][0];
    }
    CSimpleIni* ini() {
        auto ini = dynamic_cast<CCLabelTTF*>(this->getChildByIDRecursive("ini"));
        auto rtn = new CSimpleIni;
        rtn->LoadData(ini->getString());
        return rtn;
    }
    //other shit
    void onBtn(CCObject* pCCObject) {
        auto what = dynamic_cast<CCNode*>(pCCObject);
        if (not what) return;
        if (what->getID() == "reload") {
            if (checkExistence(workindir())) ghc::filesystem::remove_all(workindir());
            auto scene = CCScene::create();
            auto pModViewLayer = ModViewLayer::create(issueJson());
            scene->addChild(pModViewLayer, 0, issueJson()["number"].as_int());
            CCDirector::sharedDirector()->replaceScene(CCTransitionCrossFade::create(0.1f, scene));
        };
        if (what->getID() == "back") keyBackClicked();
        if (what->getID() == "download") {
            //endpoint
            auto endpoint = fmt::format(
                "https://github.com/{}/releases/{}/download/{}",
                ini()->GetValue("mod", "repo"), ini()->GetValue("mod", "release_tag"), ini()->GetValue("mod", "file")
            );
            auto pop = geode::MDPopup::create(
                "Download mod",
                "\n" + endpoint + "",
                "Start downloading", nullptr,
                [this](bool btn2) {
                    if (btn2);// this->downloadLatest(this);
                }
            );
            pop->show();
        };
    }
    static void openMe(matjson::Value pJson) {
        auto scene = CCScene::create();
        auto pModViewLayer = ModViewLayer::create(pJson);
        scene->addChild(pModViewLayer, 0, pJson["number"].as_int());
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
        pop->show();
    }
    void view(CCObject*) {
        auto labels_str = json["labels"].dump();
        if (labels_str.find(fmt::format("\"{}\",", "mod")) != std::string::npos) return ModViewLayer::openMe(json);
        if (labels_str.find(fmt::format("\"{}\",", "pack")) != std::string::npos) return ModViewLayer::openMe(json);
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
            pRtn->json = pJson;
            {
                //menu
                auto menu = CCMenu::create();
                menu->setPositionY(45.f);
                menu->setID("menu");
                pRtn->addChild(menu, 10);
                //square bg
                auto pCCLayerColor = CCLayerColor::create({ 0,0,0,75 });
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
                        ccColor4B(10, 10, 10, 130),
                        ccColor4B(10, 10, 10, 90)
                    );
                    auto padding = 5.f;
                    grad->setContentSize(View->getContentSize() + CCPoint(padding, padding));
                    grad->setPosition(CCPoint(-padding, -padding) / 2);
                    View->addChild(grad, -1, 57290);
                }
                //id
                CCLabelTTF* CCLabelTTFid = CCLabelTTF::create(fmt::format("#{}", pJson["number"].as_int()).c_str(), "arial", 8.f);
                CCLabelTTFid->setOpacity(60);
                CCLabelTTFid->setHorizontalAlignment(kCCTextAlignmentRight);
                CCLabelTTFid->setAnchorPoint({ 1.0f, .0f });
                CCLabelTTFid->setPositionX(POINTING_SIZE.width - 4);
                CCLabelTTFid->setPositionY(3 - POINTING_SIZE.height);
                menu->addChild(CCLabelTTFid);
                //name and tags
                {
                    //container
                    auto container = CCNode::create();
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
                        container->addChild(tag);
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
                auto issue_ini = new CSimpleIni;
                issue_ini->LoadData(pJson["body"].as_string());
                TextArea* desc = TextArea::create(
                    fmt::format(
                        "By: {} ({})\n{}\nfile: {}, repo: {}, release_tag: {}",
                        pJson["user"]["login"].as_string(),
                        pJson["user"]["id"].as_int(),
                        issue_ini->GetValue("mod", "desc", "No description provided..."),
                        issue_ini->GetValue("mod", "file", "<cr>NO FILE!!!</c>"),
                        issue_ini->GetValue("mod", "repo", "<cr>NO REPO!!!</c>"),
                        issue_ini->GetValue("mod", "release_tag", "<cr>NO REL TAG!!!</c>")
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
class RyzenLayer : public CCLayer, DynamicScrollDelegate {
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
        if (not dynamic_cast<RyzenLayer*>(this)) return;
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
    static RyzenLayer* create(std::string withFilter = "") {
        auto rtn = new RyzenLayer();
        if (rtn) {
            rtn->autorelease();
            rtn->init();
            basicRznLayersInit(rtn, menu_selector(RyzenLayer::onBtn));
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
                        rtn, menu_selector(RyzenLayer::onBtn)
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
                        rtn, menu_selector(RyzenLayer::onBtn)
                    );
                    gj_findBtn->setID("search");
                    gj_findBtn->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 82.000f });
                    menu->addChild(gj_findBtn);//add GJ_replayBtn_001
                    //addmodbtn
                    CCMenuItemSpriteExtra* GJ_plus3Btn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png"),
                        rtn, menu_selector(RyzenLayer::onBtn)
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
        auto pRyzenLayer = RyzenLayer::create("labels:mod&");
        scene->addChild(pRyzenLayer, 1, 2816);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    };
    void openMeForPacks(cocos2d::CCObject* object) {
        auto scene = CCScene::create();
        auto pRyzenLayer = RyzenLayer::create("labels:pack&");
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
            this, menu_selector(RyzenLayer::openMe)
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
        //RyzenLayerBtn
        auto RyzenLayerBtn = CCMenuItemSpriteExtra::create(
            CCSprite::create("Ryzen_LogoBtn_001.png"_spr),
            this,
            menu_selector(RyzenLayer::openMeForPacks)
        );
        RyzenLayerBtn->m_animationEnabled = 0;
        RyzenLayerBtn->m_colorEnabled = 1;
        this->addChild(CCMenu::createWithItem(RyzenLayerBtn));
        RyzenLayerBtn->getParent()->setAnchorPoint(CCPointZero);
        RyzenLayerBtn->getParent()->setPosition(CCDirector::get()->getScreenRight(), 0.f);
        RyzenLayerBtn->getParent()->setScale(0.6f);
        RyzenLayerBtn->setPosition(-35.0f, 100.f);
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
