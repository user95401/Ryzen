#include "RyzenLayer.hpp"
#include "jute.h"
#include <urlmon.h>
#pragma comment (lib, "urlmon.lib")
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;
using namespace std;

class ModItem : public CCMenuItem {
public:
    string id;
    string name;
    string developer;
    string description;
    CCMenuItemSprite* logoBtn;
    static ModItem* create(string id) {
        ModItem* pRet = new ModItem();
        pRet->id = id;
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }
    bool init() {
        //square bg
        CCScale9Sprite* CCScale9Sprite_ = CCScale9Sprite::create("square02_small.png");
        CCScale9Sprite_->setContentSize({ 360.f, 50.f });
        CCScale9Sprite_->setOpacity(75);
        addChild(CCScale9Sprite_, -2);
        setContentSize(CCScale9Sprite_->getContentSize());
        //getModJson
        addChild(CCLabelTTF::create("Downloading mod.json ...", "arial", 8.f), 0, 8642);
        CCHttpRequest* CCHttpRequest_ = new CCHttpRequest;
        CCHttpRequest_->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        CCHttpRequest_->setUrl(("https://user95401.undo.it/Ryzen/get.php?json&id=" + id).c_str());
        CCHttpRequest_->setResponseCallback(this, httpresponse_selector(ModItem::getModJson));
        CCHttpClient::getInstance()->send(CCHttpRequest_);
        CCHttpRequest_->release();
        return true;
    }
    void setupModInfo(CCObject*) {
        CCLabelTTF* CCLabelTTFname = CCLabelTTF::create(name.c_str(), "arial", 12.f);
        CCLabelTTFname->setHorizontalAlignment(kCCTextAlignmentLeft);
        CCLabelTTFname->setAnchorPoint({0.0f, 0.5f});
        CCLabelTTFname->setPositionX(-132.000f);
        CCLabelTTFname->setPositionY(12.000f);
        addChild(CCLabelTTFname);
        CCLabelTTF* CCLabelTTFdeveloper = CCLabelTTF::create(("By: " + developer).c_str(), "arial", 8.f);
        CCLabelTTFdeveloper->setHorizontalAlignment(kCCTextAlignmentLeft);
        CCLabelTTFdeveloper->setAnchorPoint({ 0.0f, 0.5f });
        CCLabelTTFdeveloper->setPositionX(-132.000f);
        CCLabelTTFdeveloper->setPositionY(0.000f);
        addChild(CCLabelTTFdeveloper);
        CCLabelTTF* CCLabelTTFdescription = CCLabelTTF::create(description.c_str(), "arial", 6.f);
        CCLabelTTFdescription->setHorizontalAlignment(kCCTextAlignmentLeft);
        CCLabelTTFdescription->setAnchorPoint({ 0.0f, 0.5f });
        CCLabelTTFdescription->setPositionX(-132.000f);
        CCLabelTTFdescription->setPositionY(-12.000f);
        addChild(CCLabelTTFdescription);
        //logo
        logoBtn = CCMenuItemSprite::create(ModUtils::createSprite("GJ_colorBtn_001.png"), ModUtils::createSprite("GJ_colorBtn_001.png"), this, nullptr);
        logoBtn->setPositionX(-155.000f);
        logoBtn->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.3, 90), CCFadeTo::create(0.3, 160), nullptr)));
        addChild(logoBtn);
        //getModLogo
        CCHttpRequest* CCHttpRequest_ = new CCHttpRequest;
        CCHttpRequest_->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        CCHttpRequest_->setUrl(("https://user95401.undo.it/Ryzen/get.php?logo&id=" + id).c_str());
        CCHttpRequest_->setResponseCallback(this, httpresponse_selector(ModItem::getModLogo));
        CCHttpClient::getInstance()->send(CCHttpRequest_);
        CCHttpRequest_->release();
    }
    void getModJson(CCHttpClient* client, CCHttpResponse* response) {
        //get response str
        std::vector<char>* responseData = response->getResponseData();
        std::string responseString(responseData->begin(), responseData->end());
        //respose tests
        removeChildByTag(8642);//fail to get
        if (responseString == "") return addChild(CCLabelTTF::create(("Failed to get mod.json for " + id + "...").c_str(), "arial", 8.f), 0, 734);
        filesystem::create_directories(filesystem::path("geode/temp/Ryzen/" + id));
        if (S_OK == URLDownloadToFile(NULL,
            responseString.c_str(),
            ("geode/temp/Ryzen/" + id + "/mod.json").c_str(),
            0, NULL))
        {
            std::ifstream file("geode/temp/Ryzen/" + id + "/mod.json");
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
            name = (jute::parser::parse(content)["name"].as_string());
            developer = (jute::parser::parse(content)["developer"].as_string());
            description = (jute::parser::parse(content)["description"].as_string());
        }
        else {
            addChild(CCLabelTTF::create(("Failed to download mod.json from \n" + responseString).c_str(), "arial", 8.f), 0, 521);
            name = "";
            developer = "";
            description = "";
        }
        setupModInfo(nullptr);
    }
    void getModLogo(CCHttpClient* client, CCHttpResponse* response) {
        logoBtn->stopAllActions();
        logoBtn->runAction(CCFadeTo::create(0.1f, 90));
        //get response str
        std::vector<char>* responseData = response->getResponseData();
        std::string responseString(responseData->begin(), responseData->end());
        //search path
        filesystem::create_directories(filesystem::path("geode/temp/Ryzen/" + id));
        CCFileUtils::sharedFileUtils()->addSearchPath(filesystem::path("geode/temp/Ryzen/" + id).string().c_str());
        if (S_OK == URLDownloadToFile(NULL,
            responseString.c_str(),
            ("geode/temp/Ryzen/" + id + "/" + id + ".png").c_str(), 
            0, NULL))
        {
            CCTextureCache::sharedTextureCache()->reloadTexture((id + ".png").c_str());
            CCSprite* logo = ModUtils::createSprite((id + ".png").c_str());
            if (logo->getContentSize().width > logo->getContentSize().height)
                logo->setScale(logoBtn->getContentSize().width / logo->getContentSize().width);
            else
                logo->setScale(logoBtn->getContentSize().height / logo->getContentSize().height);
            logo->setPosition({ logoBtn->getContentSize().width / 2, logoBtn->getContentSize().height / 2 });
            logoBtn->addChild(logo);
            CCSprite* hiddenNode = dynamic_cast<CCSprite*>(logoBtn->getNormalImage());
            hiddenNode->runAction(CCRepeatForever::create(CCFadeOut::create(0.f)));
            logoBtn->setNormalImage(hiddenNode);
            logoBtn->setSelectedImage(hiddenNode);
            logoBtn->runAction(CCFadeIn::create(0.1f));
        }
        else {
        }
    }
};

void RyzenLayer::getModsResponse(CCHttpClient* client, CCHttpResponse* response) {
    //get response str
    std::vector<char>* responseData = response->getResponseData();
    std::string responseString(responseData->begin(), responseData->end());
    //respose tests
    //if (responseString == "") return;
    //add mods
    CCMenu_mods->removeAllChildren();
    std::vector<std::string> vect = ModUtils::explode(responseString, ',');
    for (size_t i = 0; i < vect.size(); i++)
        CCMenu_mods->addChild(ModItem::create(vect[i]));
    CCMenu_mods->alignItemsVerticallyWithPadding(10.0f);
    removeChildByTag(6302);//loadinglayer
    removeChildByTag(8732);//ErrorContainer
    if (CCMenu_mods->getChildrenCount() == 0) {
        CCNode* ErrorContainer = CCNode::create();
        ErrorContainer->setPosition(CCMenu::create()->getPosition());
        CCScale9Sprite* CCScale9Sprite_ = CCScale9Sprite::create("square02_small.png");
        CCScale9Sprite_->setContentSize({ 360.000f, 290.000f });
        CCScale9Sprite_->setOpacity(100);
        ErrorContainer->addChild(CCScale9Sprite_);
        auto deleteFilter_none_001 = ModUtils::createSprite("deleteFilter_none_001.png");
        deleteFilter_none_001->setScale(2.0f);
        ErrorContainer->addChild(deleteFilter_none_001);
        ErrorContainer->addChild(CCLabelTTF::create("\n \n \n \n \n \nCant get any mods", "arial", 12.f));
        addChild(ErrorContainer, 10, 8732);
    }
}

void RyzenLayer::getMods(CCObject*) {
    filesystem::remove_all("geode/temp/Ryzen/");
    if (getChildByTag(6302)) return;//LoadingContainer
    CCHttpRequest* CCHttpRequest_ = new CCHttpRequest;
    CCHttpRequest_->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
    CCHttpRequest_->setUrl(("https://user95401.undo.it/Ryzen/get.php?list&page=" + to_string(Page)).c_str());
    CCHttpRequest_->setResponseCallback(this, httpresponse_selector(RyzenLayer::getModsResponse));
    CCHttpClient::getInstance()->send(CCHttpRequest_);
    CCHttpRequest_->release();
    UpdatePageLbl(PageLbl);
    //LoadingCircle_->fadeAndRemove();
    //LoadingCircle_->show();
    removeChildByTag(8732);//ErrorContainer
    if (CCNode* LoadingContainer = CCNode::create()) {
        LoadingContainer->setPosition(CCMenu::create()->getPosition());
        CCScale9Sprite* CCScale9Sprite_ = CCScale9Sprite::create("square02_small.png");
        CCScale9Sprite_->setContentSize({ 360.000f, 290.000f });
        CCScale9Sprite_->setOpacity(100);
        LoadingContainer->addChild(CCScale9Sprite_);
        auto loadingCircle = ModUtils::createSprite("loadingCircle-hd.png", 2.8f);
        loadingCircle->setBlendFunc({ GL_ONE, GL_ONE });
        loadingCircle->runAction(CCRepeatForever::create(CCEaseSineOut::create(CCRotateBy::create(0.3, 360))));
        LoadingContainer->addChild(loadingCircle);
        LoadingContainer->addChild(CCLabelTTF::create("\n \n \n \nLoading mods list...", "arial", 16.f));
        addChild(LoadingContainer, 10, 6302);
    }
}

void RyzenLayer::PageUp(CCObject* object) {
    if (getChildByTag(6302)) return;//LoadingContainer
    ++Page;
    getMods(CCMenu_mods);
}

void RyzenLayer::PageDown(CCObject* object) {
    if (getChildByTag(6302)) return;//LoadingContainer
    --Page;
    getMods(CCMenu_mods);
}

void RyzenLayer::UpdatePageLbl(CCObject* object) {
    PageLbl->setString((to_string(Page) + "th page").c_str());
}

void RyzenLayer::OpenUpSearchSetup(CCObject* object) {
    if (SearchLayer->numberOfRunningActions() > 0) return;
    if (SearchLayer->getChildByTag(1642)) {
        SearchLayer->removeChildByTag(1642);
        SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3f, CCPoint(CCDirector::sharedDirector()->getWinSize().width, SearchLayer->getPositionY()))));
        SearchLayer_leftBtn->runAction(CCEaseExponentialOut::create(CCRotateTo::create(0.3f, 0.000f)));
        SquareShadow->runAction(CCEaseExponentialOut::create(CCFadeOut::create(0.3f)));
    }
    else {
        SearchLayer->addChild(CCNode::create(), 1, 1642);
        SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3f, CCPoint(CCDirector::sharedDirector()->getScreenLeft() + (SearchLayer_leftBtn->getContentSize().width*2)/0.5, SearchLayer->getPositionY()))));
        SearchLayer_leftBtn->runAction(CCEaseExponentialOut::create(CCRotateTo::create(0.3f, 180.000f)));
        SquareShadow->runAction(CCEaseExponentialOut::create(CCFadeIn::create(0.3f)));
    }
}

bool RyzenLayer::init() {
    setKeypadEnabled(true);
    setTouchEnabled(true);
    //play music
    GameManager::sharedState()->fadeInMusic("Graham Kartna - Browser History.mp3");
    //bg
    CCSprite* backgroundSprite = ModUtils::createSprite("GJ_gradientBG.png");
    backgroundSprite->setScaleX(CCDirector::sharedDirector()->getWinSize().width / backgroundSprite->getContentSize().width);
    backgroundSprite->setScaleY(CCDirector::sharedDirector()->getWinSize().height / backgroundSprite->getContentSize().height);
    backgroundSprite->setAnchorPoint({ 0, 0 });
    backgroundSprite->setColor({ 70, 80, 90 });
    backgroundSprite->runAction(CCRepeatForever::create(CCSequence::create(CCTintTo::create(5.0, 70, 80, 90), CCTintTo::create(5.0, 60, 70, 80), CCTintTo::create(5.0, 70, 80, 80), nullptr)));
    addChild(backgroundSprite, -2);
    //SquareShadowCorner1
    CCSprite* SquareShadowCorner1 = ModUtils::createSprite("groundSquareShadow_001.png");
    SquareShadowCorner1->setScaleX(0.080);
    SquareShadowCorner1->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner1->getContentSize().height);
    SquareShadowCorner1->setAnchorPoint({ 0, 0 });
    addChild(SquareShadowCorner1, 10);
    //SquareShadowCorner2
    CCSprite* SquareShadowCorner2 = ModUtils::createSprite("groundSquareShadow_001.png");
    SquareShadowCorner2->setScaleX(-0.080);
    SquareShadowCorner2->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner2->getContentSize().height);
    SquareShadowCorner2->setAnchorPoint({ 0, 0 });
    SquareShadowCorner2->setPositionX(CCDirector::sharedDirector()->getScreenRight());
    addChild(SquareShadowCorner2, 10);
    //SquareShadow
    SquareShadow = ModUtils::createSprite("groundSquareShadow_001.png");
    SquareShadow->setScaleX(CCDirector::sharedDirector()->getWinSize().width / SquareShadow->getContentSize().width);
    SquareShadow->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadow->getContentSize().height);
    SquareShadow->setAnchorPoint({ 0, 0 });
    SquareShadow->runAction(CCFadeOut::create(0.0f));
    addChild(SquareShadow, 10);
    //gauntletCorner_001
    CCSprite* gauntletCorner_001 = ModUtils::createSprite("gauntletCorner_001.png");
    gauntletCorner_001->setPosition({0, 0});
    gauntletCorner_001->setRotation(0);
    gauntletCorner_001->setAnchorPoint({ 0.f,0.f });
    addChild(gauntletCorner_001);//add gauntletCorner_001
    //gauntletCorner_002
    CCSprite* gauntletCorner_002 = ModUtils::createSprite("gauntletCorner_001.png");
    gauntletCorner_002->setPosition({
        CCDirector::sharedDirector()->getWinSize().width,
        0.f
        });
    gauntletCorner_002->setScaleX(-1.f);
    gauntletCorner_002->setAnchorPoint({ 0.f,0.f });
    addChild(gauntletCorner_002);//add gauntletCorner_002
    //gauntletCorner_003
    CCSprite* gauntletCorner_003 = ModUtils::createSprite("gauntletCorner_001.png");
    gauntletCorner_003->setPosition({
        (CCDirector::sharedDirector()->getWinSize().width),
        (CCDirector::sharedDirector()->getWinSize().height)
        });
    gauntletCorner_003->setRotation(180.f);
    gauntletCorner_003->setAnchorPoint({ 0.f,0.f });
    addChild(gauntletCorner_003);//add gauntletCorner_003
    //menu
    CCMenu_ = CCMenu::create();
    addChild(CCMenu_);
    //backBtn
    auto menuBack = CCMenu::createWithItem(
        CCMenuItemSpriteExtra::create(ModUtils::createSprite("GJ_arrow_03_001.png"), this, menu_selector(RyzenLayer::onBack))
    );
    menuBack->setPosition({ 25, CCDirector::sharedDirector()->getWinSize().height - 25 });
    addChild(menuBack);
    //Page
    Page = Page < 1 ? 1 : Page;//if Page<1 Page=1
    PageLbl = CCLabelTTF::create((to_string(Page) + "th page").c_str(), "Arial", 10.f);
    PageLbl->setAnchorPoint({ 0.5f, -0.15f });
    PageLbl->setPosition({ CCDirector::sharedDirector()->getWinSize().width/2, 0.f });
    addChild(PageLbl);//add lbl to layer
    //mods menu
    CCMenu_mods = CCMenu::create();
    addChild(CCMenu_mods);
    getMods(CCMenu_mods);
    //pagebtns
    CCMenu* CCMenuPage = CCMenu::create();
    addChild(CCMenuPage);//add menu to layer
    CCMenuItemSpriteExtra* edit_leftBtn_001 = CCMenuItemSpriteExtra::create(ModUtils::createSprite("edit_leftBtn_001.png"), this, menu_selector(RyzenLayer::PageDown));
    CCMenuPage->addChild(edit_leftBtn_001);//add edit_rightBtn_001
    CCMenuItemSpriteExtra* edit_rightBtn_001 = CCMenuItemSpriteExtra::create(ModUtils::createSprite("edit_rightBtn_001.png"), this, menu_selector(RyzenLayer::PageUp));
    CCMenuPage->addChild(edit_rightBtn_001);//add edit_rightBtn_001
    CCMenuPage->alignItemsHorizontallyWithPadding(360.f + 20);
    //reloadmodsbtn
    CCMenuItemSpriteExtra* GJ_replayBtn_001 = CCMenuItemSpriteExtra::create(ModUtils::createSprite("GJ_replayBtn_001.png"), this, menu_selector(RyzenLayer::getMods));
    GJ_replayBtn_001->setPosition({
        (CCDirector::sharedDirector()->getWinSize().width / 2) - 35,
        (CCDirector::sharedDirector()->getWinSize().height / -2) + 35
        });
    GJ_replayBtn_001->CCMenuItemSpriteExtra::setScale(0.7f);
    CCMenu_->addChild(GJ_replayBtn_001);//add GJ_replayBtn_001
    //search layer
    if (SearchLayer = CCLayer::create()) {
        SearchLayer->setPositionX(CCDirector::sharedDirector()->getWinSize().width);
        addChild(SearchLayer, 10, 3206);
        CCMenu* SearchLayer_Menu = CCMenu::create();
        SearchLayer_Menu->setPositionX(0.0f);
        SearchLayer->addChild(SearchLayer_Menu);
        CCScale9Sprite* CCScale9Sprite_ = CCScale9Sprite::create("square01_001.png");
        CCScale9Sprite_->setContentSize({ 1360.000f, 300.000f });
        CCScale9Sprite_->setAnchorPoint({ 0.0f, 0.5f });
        SearchLayer_Menu->addChild(CCScale9Sprite_);
        //arrow
        auto SearchLayer_leftBtn_spr = ModUtils::createSprite("edit_leftBtn_001.png");
        SearchLayer_leftBtn_spr->setScaleX(0.600f);
        SearchLayer_leftBtn = CCMenuItemSpriteExtra::create(SearchLayer_leftBtn_spr, this, menu_selector(RyzenLayer::OpenUpSearchSetup));
        SearchLayer_leftBtn->setPositionX(-(SearchLayer_leftBtn->getContentSize().width + 6.0f));
        SearchLayer_leftBtn->CCMenuItemSpriteExtra::setScale(1.5f);
        SearchLayer_Menu->addChild(SearchLayer_leftBtn);
    }
    return true;
}

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

void RyzenLayer::openMe(CCObject* object) {
    auto scene = CCScene::create();
    scene->addChild(create());
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}
