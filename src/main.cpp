#include <Geode/Geode.hpp>
using namespace geode::prelude;

class RyzenLayer : public CCLayer {
public:
    class ModItem : public CCMenuItem {
    public:
        matjson::Value json;
        std::string id;
        static ModItem* create(std::string sId) {
            auto pRet = new ModItem();
            if (pRet) {
                pRet->id = sId;
                pRet->autorelease();
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
                    CCMenuItemSpriteExtra* Ryzen_InfoBtn_001 = CCMenuItemSpriteExtra::create(CCSprite::create("Ryzen_InfoBtn_001.png"_spr), pRet, /*menu_selector(ModItem::openModInfoPage)*/nullptr);
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
    void loadMods() {
        auto modsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("modsMenu"));
        modsMenu->addChild(ModItem::create("mod1.asd"));
        modsMenu->addChild(ModItem::create("mod2.asd"));
        modsMenu->addChild(ModItem::create("mod3.asd"));
        modsMenu->addChild(ModItem::create("mod4.asd"));
        modsMenu->addChild(ModItem::create("mod5.asd"));
        modsMenu->alignItemsVerticallyWithPadding(10.0f);
    };
    static RyzenLayer* create() {
        auto ret = new RyzenLayer();
        if (ret) {
            ret->autorelease();
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
                    /**/
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
                ret->loadMods();
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
        scene->addChild(create());
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

class ModListLayer : public CCLayer, public TextInputDelegate {};
class PackSelectLayer : public CCLayer {};
#include <Geode/modify/CCLayer.hpp>
class $modify(CCLayerExt, CCLayer) {
    bool init() {
        auto rtn = CCLayer::init();
        //ModListLayer
        ModListLayer* pModListLayer = typeinfo_cast<ModListLayer*>(this);
        if (pModListLayer) {
            //RyzenLayerBtn
            auto RyzenLayerBtn = CCMenuItemSpriteExtra::create(
                CCSprite::create("logo.png"_spr),
                this,
                menu_selector(RyzenLayer::openMe)
            );
            RyzenLayerBtn->m_animationEnabled = 0;
            RyzenLayerBtn->m_colorEnabled = 1;
            pModListLayer->addChild(CCMenu::create(), 999, 5819);
            pModListLayer->getChildByTag(5819)->setAnchorPoint(CCPointZero);
            pModListLayer->getChildByTag(5819)->addChild(CCMenu::createWithItem(RyzenLayerBtn));
            RyzenLayerBtn->getParent()->setAnchorPoint(CCPointZero);
            RyzenLayerBtn->getParent()->setPosition(-210.f, -33.f);
            RyzenLayerBtn->getParent()->setScale(0.625f);
            RyzenLayerBtn->getParent()->setZOrder(10);
        }
        //PackSelectLayer
        PackSelectLayer* pPackSelectLayer = typeinfo_cast<PackSelectLayer*>(this);
        if (pPackSelectLayer) {
            //RyzenLayerBtn
            auto RyzenLayerBtn = CCMenuItemSpriteExtra::create(
                CCSprite::create("logo.png"_spr),
                this,
                menu_selector(RyzenLayer::openMe)
            );
            RyzenLayerBtn->m_animationEnabled = 0;
            RyzenLayerBtn->m_colorEnabled = 1;
            pPackSelectLayer->addChild(CCMenu::create(), 999, 5819);
            pPackSelectLayer->getChildByTag(5819)->setAnchorPoint(CCPointZero);
            pPackSelectLayer->getChildByTag(5819)->addChild(CCMenu::createWithItem(RyzenLayerBtn));
            RyzenLayerBtn->getParent()->setAnchorPoint(CCPointZero);
            RyzenLayerBtn->getParent()->setPosition(-18.f, 110.f);
            RyzenLayerBtn->getParent()->setScale(0.35f);
            RyzenLayerBtn->getParent()->setZOrder(10);
        }
        return rtn;
    };
};
