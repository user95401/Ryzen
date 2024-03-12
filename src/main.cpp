#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
using namespace geode::prelude;

class RyzenLayer : public CCLayer {
public:
    int p = 0;
    std::string q;
    class ModItem : public CCMenuItem {
    public:
        matjson::Value json;
        std::string id;
        static ModItem* create(std::string sId) {
            auto pRet = new ModItem();
            if (pRet) {
                pRet->id = sId;
                pRet->autorelease();
                pRet->init();
                {
                    //square bg
                    CCScale9Sprite* CCScale9Sprite_ = CCScale9Sprite::create("square02_small.png");
                    CCScale9Sprite_->setContentSize({ 360.f, 50.f });
                    CCScale9Sprite_->setOpacity(75);
                    pRet->addChild(CCScale9Sprite_, -2);
                    pRet->setContentSize(CCScale9Sprite_->getContentSize());
                    //menu
                    auto menu = CCMenu::create();
                    menu->setAnchorPoint(CCPointZero);
                    menu->setPosition(CCPointZero);
                    menu->setContentSize(pRet->getContentSize());
                    pRet->addChild(menu, 10);
                    //Ryzen_InfoBtn_001
                    CCMenuItemSpriteExtra* Ryzen_InfoBtn_001 = CCMenuItemSpriteExtra::create(CCSprite::create("Ryzen_InfoBtn_001.png"_spr), pRet, nullptr);
                    Ryzen_InfoBtn_001->setPositionX(172.000f);
                    Ryzen_InfoBtn_001->setPositionY(16.000f);
                    menu->addChild(Ryzen_InfoBtn_001);
                    //id
                    CCLabelTTF* CCLabelTTFid = CCLabelTTF::create(pRet->id.c_str(), "arial", 6.f);
                    CCLabelTTFid->setOpacity(60);
                    CCLabelTTFid->setHorizontalAlignment(kCCTextAlignmentRight);
                    CCLabelTTFid->setAnchorPoint({ 1.0f, 1.0f });
                    CCLabelTTFid->setPositionX(177.000f);
                    CCLabelTTFid->setPositionY(-16.000f);
                    menu->addChild(CCLabelTTFid);
                }
                return pRet;
            }
            else {
                delete pRet;
                pRet = NULL;
                return NULL;
            }
        }
    };
    void onBtn(CCObject* pCCObject) {
        auto what = dynamic_cast<CCNode*>(pCCObject);
        if (not what) return;
        if (what->getID() == "reload") loadMods(0.f);
        if (what->getID() == "OpenUpPageSetup") {
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            auto PageSetupLayer = typeinfo_cast<CCLayer*>(this->getChildByIDRecursive("PageSetupLayer"));
            if (not PageLbl) return;
            if (not PageSetupLayer) return;
            if (PageSetupLayer->numberOfRunningActions() > 0) return;
            if (PageSetupLayer->getChildByTag(138)) {
                PageSetupLayer->removeChildByTag(138);
                PageSetupLayer->runAction(CCEaseBackIn::create(CCMoveTo::create(0.3f, CCPoint(PageSetupLayer->getPositionX(), -120.f))));
                PageLbl->setOpacity(90);
            }
            else {
                PageSetupLayer->addChild(CCNode::create(), 1, 138);
                PageSetupLayer->runAction(CCEaseBackOut::create(CCMoveTo::create(0.3f, CCPoint(PageSetupLayer->getPositionX(), 0.f))));
                PageLbl->setOpacity(180);
            }
        }
        if (what->getID() == "gotopage") {
            auto PageInput = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("PageInput"));
            if (not PageInput) return;
            p = (PageInput->getString().c_str() == "") ? 1 : atoi(PageInput->getString().data());
            p = p > 0 ? p : 0;
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            if (PageLbl) PageLbl->setString(fmt::format("{}th page", p).c_str());
            loadMods(0.f);
        };
        if (what->getID() == "PageDown") {
            --p;
            p = p > 0 ? p : 0;
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            if (PageLbl) PageLbl->setString(fmt::format("{}th page", p).c_str());
            loadMods(0.f);
        }
        if (what->getID() == "PageUp") {
            ++p;
            p = p > 0 ? p : 0;
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            if (PageLbl) PageLbl->setString(fmt::format("{}th page", p).c_str());
            loadMods(0.f);
        }
        //CC_SAFE_RELEASE(pCCObject);
    }
    void loadMods(float) {
        auto modsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("modsMenu"));
        if (!modsMenu->getChildByTag(580)) modsMenu->addChild(
            Notification::create("Loading...", NotificationIcon::Loading), 1, 580
        );
        web::AsyncWebRequest()
            .fetch(fmt::format("http://ryzen.user95401.undo.it/api/mods-list.php?p={}&q={}", p, q))
            .text()
            .then([this, modsMenu](std::string const& data)
                {
                    auto BtnCmd = CCNode::create();
                    BtnCmd->setID("PageDown");
                    if (data.empty()) {
                        if (p > 0) 
                            this->onBtn(BtnCmd);
                        if (!modsMenu->getChildByTag(521)) modsMenu->addChild(
                            Notification::create("Mods wasnt founded...", NotificationIcon::Error), 1, 521
                        );
                    }
                    else {
                        // parse resulting data :3
                        std::string tmp;
                        std::stringstream ss(data);
                        std::vector<std::string> mods;
                        while (std::getline(ss, tmp, ',')) {
                            mods.push_back(tmp);
                        }
                        modsMenu->removeAllChildren();
                        for (auto mod : mods) {
                            auto entry = ModItem::create(mod);
                            if (rand() % 2 == 0)
                                entry->runAction(CCSequence::create(
                                    CCMoveBy::create(0.0f, CCPoint(500, 0)),
                                    CCEaseExponentialOut::create(CCMoveBy::create(0.5f, CCPoint(-500, 0))),
                                    nullptr
                                ));
                            else
                                entry->runAction(CCSequence::create(
                                    CCMoveBy::create(0.0f, CCPoint(-500, 0)),
                                    CCEaseExponentialOut::create(CCMoveBy::create(0.5f, CCPoint(500, 0))),
                                    nullptr
                                ));
                            modsMenu->addChild(entry);
                        }
                        modsMenu->alignItemsVerticallyWithPadding(10.0f);
                    };
                }
            )
            .expect([](std::string const& error) {
                    // something went wrong with our web request Q~Q
                });
    };
    static RyzenLayer* create() {
        auto ret = new RyzenLayer();
        if (ret) {
            ret->autorelease();
            ret->init();
            {
                {
                    //play music
                    GameManager::sharedState()->fadeInMusic("Owady nocy.mp3"_spr);
                    //setup
                    ret->setKeypadEnabled(true);
                    ret->setTouchEnabled(true);
                    //bg
                    CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
                    backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
                    backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
                    backgroundSprite->setAnchorPoint({ 0, 0 });
                    backgroundSprite->setColor({ 70, 80, 90 });
                    backgroundSprite->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(5.0, 70, 80, 90), CCTintTo::create(5.0, 60, 70, 80), CCTintTo::create(5.0, 70, 80, 80), nullptr)));
                    ret->addChild(backgroundSprite, -2);
                    //SquareShadowCorner1
                    CCSprite* SquareShadowCorner1 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                    SquareShadowCorner1->setScaleX(0.050f);
                    SquareShadowCorner1->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner1->getContentSize().height);
                    SquareShadowCorner1->setAnchorPoint({ 0, 0 });
                    ret->addChild(SquareShadowCorner1, -1);
                    //SquareShadowCorner2
                    CCSprite* SquareShadowCorner2 = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
                    SquareShadowCorner2->setScaleX(-0.050f);
                    SquareShadowCorner2->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner2->getContentSize().height);
                    SquareShadowCorner2->setAnchorPoint({ 0, 0 });
                    SquareShadowCorner2->setPositionX(CCDirector::sharedDirector()->getScreenRight());
                    ret->addChild(SquareShadowCorner2, -1);
                    //gauntletCorner_001
                    CCSprite* gauntletCorner_001 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
                    gauntletCorner_001->setPosition({ 0, 0 });
                    gauntletCorner_001->setRotation(0);
                    gauntletCorner_001->setAnchorPoint({ 0.f,0.f });
                    ret->addChild(gauntletCorner_001);//add gauntletCorner_001
                    //gauntletCorner_002
                    CCSprite* gauntletCorner_002 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
                    gauntletCorner_002->setPosition({
                        CCDirector::sharedDirector()->getWinSize().width,
                        0.f
                        });
                    gauntletCorner_002->setScaleX(-1.f);
                    gauntletCorner_002->setAnchorPoint({ 0.f,0.f });
                    ret->addChild(gauntletCorner_002);//add gauntletCorner_002
                    //gauntletCorner_003
                    CCSprite* gauntletCorner_003 = CCSprite::createWithSpriteFrameName("gauntletCorner_001.png");
                    gauntletCorner_003->setPosition({
                        (CCDirector::sharedDirector()->getWinSize().width),
                        (CCDirector::sharedDirector()->getWinSize().height)
                        });
                    gauntletCorner_003->setRotation(180.f);
                    gauntletCorner_003->setAnchorPoint({ 0.f,0.f });
                    ret->addChild(gauntletCorner_003);//add gauntletCorner_003
                    //backBtn
                    auto menuBack = CCMenu::createWithItem(
                        CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), ret, menu_selector(RyzenLayer::onBack))
                    );
                    menuBack->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
                    ret->addChild(menuBack);
                };
                //mods menu
                auto modsMenu = CCMenu::create();
                modsMenu->setID("modsMenu");
                ret->addChild(modsMenu);
                ret->loadMods(0.f);
                //menu
                auto menu = CCMenu::create();
                ret->addChild(menu);
                //toprightbar
                {
                    //reloadmodsbtn
                    CCMenuItemSpriteExtra* Ryzen_ReloadBtn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::create("Ryzen_ReloadBtn_001.png"_spr),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    Ryzen_ReloadBtn_001->setID("reload");
                    Ryzen_ReloadBtn_001->setPosition({
                        (CCDirector::sharedDirector()->getWinSize().width / 2) - 35,
                        (CCDirector::sharedDirector()->getWinSize().height / -2) + 35
                        });
                    Ryzen_ReloadBtn_001->getNormalImage()->setScale(0.7f);
                    menu->addChild(Ryzen_ReloadBtn_001);//add GJ_replayBtn_001
                    //gj_findBtn
                    auto gj_findBtn = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    gj_findBtn->setID("find");
                    gj_findBtn->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 82.000f });
                    menu->addChild(gj_findBtn);//add GJ_replayBtn_001
                    //addmodbtn
                    CCMenuItemSpriteExtra* GJ_plus3Btn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    gj_findBtn->setID("add_link");
                    GJ_plus3Btn_001->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 46.000f });
                    GJ_plus3Btn_001->getNormalImage()->setScale(1.700f);
                    menu->addChild(GJ_plus3Btn_001);//add GJ_replayBtn_001
                };
                //pagination
                {
                    //pagebtns
                    CCMenu* CCMenuPage = CCMenu::create();
                    CCMenuPage->setID("CCMenuPage");
                    ret->addChild(CCMenuPage);//add menu to layer
                    CCMenuItemSpriteExtra* edit_leftBtn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    edit_leftBtn_001->setID("PageDown");
                    CCMenuPage->addChild(edit_leftBtn_001);//add edit_rightBtn_001
                    CCMenuItemSpriteExtra* edit_rightBtn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    edit_rightBtn_001->setID("PageUp");
                    CCMenuPage->addChild(edit_rightBtn_001);//add edit_rightBtn_001
                    CCMenuPage->alignItemsHorizontallyWithPadding(360.f + 20);
                    //PageLbl
                    auto PageLbl = CCMenuItemLabel::create(
                        CCLabelTTF::create(fmt::format("{}th page", ret->p).c_str(), "Arial", 10.f),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    PageLbl->setID("OpenUpPageSetup");
                    PageLbl->getLabel()->setID("PageLbl");
                    PageLbl->setAnchorPoint({ 0.5f, -0.15f });
                    PageLbl->setPositionY(CCDirector::sharedDirector()->getWinSize().height / -2);
                    PageLbl->setOpacity(90);
                    CCMenuPage->addChild(PageLbl);//add lbl to menu
                };
                //layers
                {
                    //PageSetupLayer
                    auto PageSetupLayer = CCLayer::create();
                    if (PageSetupLayer->init()) {
                        //add and setup
                        ret->addChild(PageSetupLayer, 20, 621);
                        PageSetupLayer->setID("PageSetupLayer");
                        PageSetupLayer->runAction(CCEaseBackInOut::create(CCMoveTo::create(0.5f, CCPoint(PageSetupLayer->getPositionX(), -120.f))));
                        //menu
                        CCMenu* PageSetupLayer_Menu = CCMenu::create();
                        PageSetupLayer_Menu->setPositionY(28.000f);
                        PageSetupLayer->addChild(PageSetupLayer_Menu);
                        //idInput
                        auto PageInput = CCTextInputNode::create(90.f, 20.f, "", 12, "chatFont.fnt");//"developer.modname"
                        PageInput->m_textField->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
                        PageInput->setAllowedChars("0123456789");
                        PageInput->setID("PageInput");
                        PageSetupLayer_Menu->addChild(PageInput);
                        //PageInputbtn1
                        auto PageInputbtn1Sprite = ButtonSprite::create("Hop to", 0, false, "goldFont.fnt", "GJ_button_05.png", 0, 0.6);
                        auto PageInputbtn1 = CCMenuItemSpriteExtra::create(PageInputbtn1Sprite, ret, menu_selector(RyzenLayer::onBtn));
                        PageInputbtn1->setID("gotopage");
                        PageInputbtn1->setPositionY(30.000f);
                        PageInputbtn1->setPositionX(158.000f);
                        PageInputbtn1->CCMenuItemSpriteExtra::setScale(0.900f);
                        PageSetupLayer_Menu->addChild(PageInputbtn1);
                        //alignItems
                        PageSetupLayer_Menu->alignItemsHorizontallyWithPadding(12.f);
                        //inputbg
                        CCScale9Sprite* PageInputBG = CCScale9Sprite::create("square02_001.png");
                        PageInputBG->setContentSize({ PageInput->getContentSize().width * 2 + 20, PageInput->getContentSize().height * 2 + 15 });
                        PageInputBG->setScale(0.5f);
                        PageInputBG->setOpacity(60);
                        PageInputBG->setPosition(PageInput->getPosition());
                        PageSetupLayer_Menu->addChild(PageInputBG, -1);
                    }
                }
            };
        }
        else {
            delete ret;
            ret = nullptr;
        }
        return ret;
    }
    void openMe(cocos2d::CCObject* object) {
        auto scene = CCScene::create();
        scene->addChild(RyzenLayer::create(), 1, 2816);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    };
    void openMeForPacks(cocos2d::CCObject* object) {
        auto scene = CCScene::create();
        scene->addChild(RyzenLayer::create(), 1, 2816);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    };
    void onBack(CCObject* object) {
        keyBackClicked();
    }
    void keyBackClicked() {
        GameManager::sharedState()->fadeInMenuMusic();
        CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }
};

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayerExt, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		return true;
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
