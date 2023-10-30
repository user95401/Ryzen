#include "RyzenLayer.hpp"
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

RyzenLayer* RyzenLayer::create() {
    auto ret = new RyzenLayer();
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void RyzenLayer::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    GameManager::sharedState()->fadeInMusic("menuLoop.mp3");
}

void RyzenLayer::onBack(CCObject* object) {
    keyBackClicked();
}

bool RyzenLayer::init() {
    setKeypadEnabled(true);
    setTouchEnabled(true);
    //play music
    GameManager::sharedState()->fadeInMusic("Graham Kartna - Browser History.mp3");
    //bg
    auto backgroundSprite = ModUtils::createSprite("GJ_gradientBG.png");
    auto size = backgroundSprite->getContentSize();
    backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / size.width);
    backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / size.height);
    backgroundSprite->setAnchorPoint({ 0, 0 });
    backgroundSprite->setZOrder(-2);
    backgroundSprite->setColor({ 70, 80, 90 });
    backgroundSprite->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(5.0, 70, 80, 90), CCTintTo::create(5.0, 60, 70, 80), CCTintTo::create(5.0, 70, 80, 80), nullptr)));
    addChild(backgroundSprite);
    //menu
    CCMenu* CCMenu_ = CCMenu::create();
    addChild(CCMenu_);
    //backBtn
    auto menuBack = CCMenu::createWithItem(
        CCMenuItemSpriteExtra::create(ModUtils::createSprite("GJ_arrow_03_001.png"), this, menu_selector(RyzenLayer::onBack))
    );
    menuBack->setPosition({25, CCDirector::sharedDirector()->getWinSize().height - 25});
    addChild(menuBack);
    //mods menu
    return true;
}

void RyzenLayer::openMe(CCObject* object) {
    auto scene = CCScene::create();
    scene->addChild(create());
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}