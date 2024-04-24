#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
using namespace geode::prelude;

class ModItem : public CCMenuItem {
public:
    matjson::Value json;
    static ModItem* create(matjson::Value pJson) {
        auto pRtn = new ModItem();
        if (pRtn) {
            pRtn->init();
            pRtn->json = pJson;
            {
                {
                    //menu
                    auto menu = CCMenu::create();
                    menu->setContentSize(pRtn->getContentSize());
                    menu->setPositionY(45.f);
                    menu->setID("menu");
                    pRtn->addChild(menu, 10);
                    //square bg
                    CCScale9Sprite* CCScale9Sprite_ = CCScale9Sprite::create("square02_small.png");
                    CCScale9Sprite_->setContentSize({ CCDirector::get()->getScreenRight(), 90.f });
                    CCScale9Sprite_->setOpacity(75);
                    pRtn->setContentSize(CCScale9Sprite_->getContentSize());
                    menu->addChild(CCScale9Sprite_, -1);
                    //Ryzen_InfoBtn_001
                    CCMenuItemSpriteExtra* Ryzen_InfoBtn_001 = CCMenuItemSpriteExtra::create(CCSprite::create("Ryzen_InfoBtn_001.png"_spr), pRtn, nullptr);
                    Ryzen_InfoBtn_001->setPositionX(172.000f);
                    Ryzen_InfoBtn_001->setPositionY(16.000f);
                    menu->addChild(Ryzen_InfoBtn_001);
                    //id
                    CCLabelTTF* CCLabelTTFid = CCLabelTTF::create(pJson["id"].as_string().c_str(), "arial", 6.f);
                    CCLabelTTFid->setOpacity(60);
                    CCLabelTTFid->setHorizontalAlignment(kCCTextAlignmentRight);
                    CCLabelTTFid->setAnchorPoint({ 1.0f, 1.0f });
                    CCLabelTTFid->setPositionX(177.000f);
                    CCLabelTTFid->setPositionY(-16.000f);
                    menu->addChild(CCLabelTTFid);
                    //name
                    CCLabelTTF* name = CCLabelTTF::create(pJson["title"].as_string().c_str(), "arial", 12.f);
                    name->setHorizontalAlignment(kCCTextAlignmentLeft);
                    name->setAnchorPoint({ 0.0f, 0.5f });
                    name->setPositionX(-132.000f);
                    name->setPositionY(12.000f);
                    menu->addChild(name);
                }
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
        if (what->getID() == "reload");
    }
    void loadMods() {
        web::AsyncWebRequest()
            .fetch("https://api.github.com/repos/user95401/Ryzen-Mods/issues")
            .json()
            .then(
                [this](matjson::Value const& catgirls) {
                    // do something with the catgirls :3
                    if (not dynamic_cast<RyzenLayer*>(this)) return;
                    auto mods_scroll = typeinfo_cast<ScrollLayer*>(this->getChildByID("mods_scroll"));
                    if (not mods_scroll) return;
                    for (auto catgirl : catgirls.as_array()) {
                        mods_scroll->m_contentLayer->addChild(ModItem::create(catgirl));
                        mods_scroll->m_contentLayer->updateLayout();
                    }
                }
            )
            .expect([this](std::string const& error) {
                    // something went wrong with our web request Q~Q
                });
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
                    //bg
                    CCSprite* backgroundSprite = CCSprite::create("GJ_gradientBG.png");
                    backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
                    backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
                    backgroundSprite->setAnchorPoint({ 0, 0 });
                    backgroundSprite->setColor({ 70, 80, 90 });
                    backgroundSprite->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(5.0, 70, 80, 90), CCTintTo::create(5.0, 60, 70, 80), CCTintTo::create(5.0, 70, 80, 80), nullptr)));
                    rtn->addChild(backgroundSprite, -2);
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
                    //backBtn
                    auto menuBack = CCMenu::createWithItem(
                        CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), rtn, menu_selector(RyzenLayer::onBack))
                    );
                    menuBack->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
                    rtn->addChild(menuBack);
                };
                //mods_scroll
                
                auto scroll_size = CCSize(CCDirector::get()->getScreenRight() - 82.f, CCDirector::get()->getScreenTop());
                auto mods_scroll = geode::ScrollLayer::create(CCDirector::get()->getWinSize());
                mods_scroll->setID("mods_scroll");
                mods_scroll->m_contentLayer->setLayout(
                    ColumnLayout::create()
                    ->setGap(12.f)
                );
                mods_scroll->setScale(0.825);
                rtn->addChild(mods_scroll);
            };
            rtn->loadMods();
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
