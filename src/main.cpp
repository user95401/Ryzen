#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "ghapiauth.h"
using namespace geode::prelude;

#include "SimpleIni.h"

class IssueItem : public CCMenuItem {
public:
    matjson::Value json;
    void onInfo(CCObject*) {
        auto asd = std::stringstream("asd");
        /**/asd << "```json";
        asd << json.dump();
        asd << "```";
        auto pop = geode::createQuickPopup("Json data", "\n \n \n \n \n \n \n \n \n \n", "Yes", nullptr, 390.f, nullptr, 0);
        TextArea* desc = TextArea::create(asd.str(), "chatFont.fnt", 0.6f, 340.f, {0.0f, 1.0f}, 10.0f, false);
        auto scroll = ScrollLayer::create({ 380.f, 260.f });
        scroll->m_contentLayer->addChild(desc);
        scroll->setPositionX(340.f / 2);
        pop->m_buttonMenu->addChild(scroll);
        pop->show();
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
                pCCLayerColor->setContentSize({ 420.f, 60.f });//THE SIZE OF ITEM GOES HERE
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
                //id
                CCLabelTTF* CCLabelTTFid = CCLabelTTF::create(fmt::format("#{}", pJson["number"].as_int()).c_str(), "arial", 8.f);
                CCLabelTTFid->setOpacity(60);
                CCLabelTTFid->setHorizontalAlignment(kCCTextAlignmentRight);
                CCLabelTTFid->setAnchorPoint({ 1.0f, .0f });
                CCLabelTTFid->setPositionX(POINTING_SIZE.width - 4);
                CCLabelTTFid->setPositionY(3 - POINTING_SIZE.height);
                menu->addChild(CCLabelTTFid);
                //name
                CCLabelTTF* name = CCLabelTTF::create(pJson["title"].as_string().c_str(), "arial", 10.f);
                name->setHorizontalAlignment(kCCTextAlignmentLeft);
                name->setAnchorPoint({ 0.0f, -0.8f });
                name->setPositionX(10 - POINTING_SIZE.width);
                /*fit up name scale if big*/ {
                    auto node = name;
                    float maxwith = 280.f;
                    if (node->getContentSize().width > maxwith) node->setScale(maxwith / node->getContentSize().width);
                };
                menu->addChild(name);
                //desc
                auto issue_ini = new CSimpleIni;
                issue_ini->LoadData(pJson["body"].as_string());
                TextArea* desc = TextArea::create(
                    std::format(
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
                menu->addChild(desc);
            }
            if (pContentLayer) pContentLayer->addChild(pRtn);
            if (pScrollLayer) {
                auto asd = pScrollLayer->getContentSize().width / pRtn->getContentSize().width;
                //log::debug("{}/{}={}", pScrollLayer->getContentSize().width, pRtn->getContentSize().width, asd);
                pRtn->setScale(asd);
            }
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
        if (what->getID() == "reload") downloadMods();
    }
    void setupMods(matjson::Value const& catgirls) {
        if (not dynamic_cast<RyzenLayer*>(this)) return;
        auto scroll = dynamic_cast<ScrollLayer*>(this->getChildByID("scroll"));
        if (not scroll) return;
        scroll->m_contentLayer->removeAllChildren();
        scroll->m_contentLayer->setContentHeight(0.f);
        auto index = 0;
        for (auto catgirl : catgirls.as_array()) {
            //filter parse
            {

            }
            //item add
            auto item = IssueItem::create(catgirl, scroll->m_contentLayer, scroll);
            //long down
            scroll->m_contentLayer->setContentHeight(//make content layer longer
                scroll->m_contentLayer->getContentHeight() + item->getContentHeight()
            );
            //add border if here next item will
            if (catgirls.as_array().size() > (index + 1)) {
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
    static RyzenLayer* create() {
        auto rtn = new RyzenLayer();
        if (rtn) {
            rtn->autorelease();
            rtn->init();
            {
                {
                    //play music
                    GameManager::sharedState()->fadeInMusic("Owady nocy.mp3"_spr);
                    //setup
                    rtn->setKeypadEnabled(true);
                    rtn->setTouchEnabled(true);
                    /*bg*/ {
                        CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
                        backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
                        backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
                        backgroundSprite->setAnchorPoint({ 0, 0 });
                        backgroundSprite->setColor({ 70, 80, 90 });
                        backgroundSprite->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(5.0, 70, 80, 90), CCTintTo::create(5.0, 60, 70, 80), CCTintTo::create(5.0, 70, 80, 80), nullptr)));
                        rtn->addChild(backgroundSprite, -2);
                    };
                    /*SquareShadowCorners*/ {
                        //SquareShadowCorner1
                        CCSprite* SquareShadowCorner1 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                        SquareShadowCorner1->setScaleX(0.050f);
                        SquareShadowCorner1->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner1->getContentSize().height);
                        SquareShadowCorner1->setAnchorPoint({ 0, 0 });
                        rtn->addChild(SquareShadowCorner1, -1);
                        //SquareShadowCorner2
                        CCSprite* SquareShadowCorner2 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                        SquareShadowCorner2->setScaleX(-0.050f);
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
                        auto menuBack = CCMenu::createWithItem(
                            CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), rtn, menu_selector(RyzenLayer::onBack))
                        );
                        menuBack->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
                        rtn->addChild(menuBack);
                    };
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
                            (CCDirector::sharedDirector()->getWinSize().width / 2) - 35,
                            (CCDirector::sharedDirector()->getWinSize().height / -2) + 35
                            });
                        reload->getNormalImage()->setScale(0.7f);
                        menu->addChild(reload);//add GJ_replayBtn_001
                        //gj_findBtn
                        auto gj_findBtn = CCMenuItemSpriteExtra::create(
                            CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"),
                            rtn, menu_selector(RyzenLayer::onBtn)
                        );
                        gj_findBtn->setID("OpenSearchBar");
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
                            1, 
                            header->getContentSize().height
                        );
                        l->setColor(ccBLACK);
                        l->setScaleY((CCDirector::get()->getScreenTop() * 2) / l->getContentSize().height);
                        CCScale9Sprite* r = CCScale9Sprite::createWithSpriteFrameName("edit_vLine_001.png");
                        header->addChild(r);
                        r->setPosition(
                            header->getContentSize().width - 1,
                            header->getContentSize().height
                        );
                        r->setColor(ccBLACK);
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
                        input->setID("input");
                        input->m_placeholderColor = ccColor3B(160, 160, 160);
                        input->m_placeholderLabel->setColor(input->m_placeholderColor);
                        input->setPosition(header->getPosition());
                        input->setPositionY(input->getPositionY() - (header->getContentSize().height / 2));
                        rtn->addChild(input);
                    }
                }
            };
            auto local_issues_file = dirs::getGeodeDir() / "ryzen" / "issues.json";
            auto local_issues = std::ifstream(local_issues_file);
            if (local_issues.is_open())
            {
                std::string line;
                std::stringstream stream;
                while (std::getline(local_issues, line)) {
                    stream << line;
                };
                rtn->setupMods(matjson::parse(stream.str()));
            }
            else rtn->downloadMods();
        }
        else {
            delete rtn;
            rtn = nullptr;
        }
        return rtn;
    }
    void openMe(cocos2d::CCObject* object) {
        auto scene = CCScene::create();
        auto pRyzenLayer = RyzenLayer::create();
        scene->addChild(pRyzenLayer, 1, 2816);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    };
    void openMeForPacks(cocos2d::CCObject* object) {
        openMe(object);
    };
    void onBack(CCObject* object) {
        keyBackClicked();
    }
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
