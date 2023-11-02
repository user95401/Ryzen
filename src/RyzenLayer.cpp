#include "RyzenLayer.hpp"
#include "jute.h"
#include "zip.h"
#include <urlmon.h>
#pragma comment (lib, "urlmon.lib")
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;
using namespace std;

string url_encode(const string& value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << setw(2) << int((unsigned char)c);
        escaped << nouppercase;
    }

    return escaped.str();
}

//try load mod
int on_extract_entry(const char* filename, void* arg) {
    static int i = 0;
    int n = *(int*)arg;
    //ModUtils::log(format("Extracted: %s (%d of %d)\n", filename, ++i, n));
    CCFileUtils::sharedFileUtils()->addSearchPath(filesystem::path(filename).parent_path().string().c_str());
    if (strstr(filename, ".dll")) {
        LoadLibraryA(filename);
        FLAlertLayer::create(nullptr, "Mod loaded", "OK", nullptr, 310, (filesystem::path(filename).filename().string() + "\n was loaded."))->show();
    }
    return 0;
}
class ModItem : public CCMenuItem {
public:
    string id;
    string name;
    string developer;
    string description;
    CCMenu* menu;
    CCMenuItemSprite* logoBtn;
    CCMenuItemSpriteExtra* Ryzen_DownloadBtn_001;
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
        //menu
        menu = CCMenu::create();
        menu->setAnchorPoint(CCPointZero);
        menu->setPosition(CCPointZero);
        menu->setContentSize(getContentSize());
        addChild(menu, 10);
        //getModJson
        addChild(CCLabelTTF::create("Downloading mod.json ...", "arial", 8.f), 0, 8642);
        CCHttpRequest* CCHttpRequest_ = new CCHttpRequest;
        CCHttpRequest_->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        CCHttpRequest_->setUrl(("https://user95401.undo.it/Ryzen/get.php?json&id=" + id).c_str());
        CCHttpRequest_->setResponseCallback(this, httpresponse_selector(ModItem::getModJson));
        CCHttpClient::getInstance()->send(CCHttpRequest_);
        CCHttpRequest_->release();
        menu->setTouchEnabled(0);
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
        //Ryzen_DownloadBtn_001
        Ryzen_DownloadBtn_001 = CCMenuItemSpriteExtra::create(ModUtils::createSprite("Ryzen_DownloadBtn_001.png", 4.0f), this, menu_selector(ModItem::getModDownload));
        Ryzen_DownloadBtn_001->setPositionX(155.000f);
        menu->addChild(Ryzen_DownloadBtn_001);
        //deletebtn if exists
        if (CCFileUtils::sharedFileUtils()->isFileExist(("geode/mods/" + id + ".geode").c_str())) {
            Ryzen_DownloadBtn_001->setNormalImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
            Ryzen_DownloadBtn_001->setSelectedImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
            Ryzen_DownloadBtn_001->setTarget(this, menu_selector(ModItem::deleteMe));
        }
    }
    void getModDownload(CCObject*) {
        Ryzen_DownloadBtn_001->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.3, 90), CCFadeTo::create(0.3, 160), nullptr)));
        //getModDownload
        CCHttpRequest* CCHttpRequest_ = new CCHttpRequest;
        CCHttpRequest_->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        CCHttpRequest_->setUrl(("https://user95401.undo.it/Ryzen/get.php?download&id=" + id).c_str());
        CCHttpRequest_->setResponseCallback(this, httpresponse_selector(ModItem::downloadMod));
        CCHttpClient::getInstance()->send(CCHttpRequest_);
        CCHttpRequest_->release();
    }
    void deleteMe(CCObject*) {
        //remove geode/mods/[id].geode
        std::remove(("geode/mods/"+id+".geode").c_str());
        ModUtils::log(("geode/mods/" + id + ".geode removed"));
        //free lib out
        //cancelled bec its crashing
        /*auto ModuleHandle = GetModuleHandleA((id + ".dll").c_str());
        if(ModuleHandle) FreeLibrary(ModuleHandle);
        else ModUtils::log(("Cant get module geode/unzipped/" + id + ".dll"));//fail
        if (ModuleHandle) ModUtils::log(("geode/unzipped/" + id + ".dll freed with exit code 0"));//log*/
        //remove geode/unzipped/[id] folder
        std::remove(("geode/unzipped/" + id + "").c_str());
        ModUtils::log(("geode/unzipped/" + id + " removed"));//log
        removeAllChildren();
        init();
        ModUtils::log(("ModItem \"" + id + "\" recreated"));//log
    }
    void getModJson(CCHttpClient* client, CCHttpResponse* response) {
        menu->setTouchEnabled(1);
        //get response str
        std::vector<char>* responseData = response->getResponseData();
        std::string responseString(responseData->begin(), responseData->end());
        //respose tests
        removeChildByTag(8642);//fail to get
        if (responseString == "") return addChild(CCLabelTTF::create(("Failed to get mod.json for " + id + "...").c_str(), "arial", 8.f), 0, 734);
        filesystem::create_directories(filesystem::path("geode/temp/Ryzen/" + id));
        /*if (S_OK == URLDownloadToFile(NULL,
            responseString.c_str(),
            ("geode/temp/Ryzen/" + id + "/mod.json").c_str(),
            0, NULL))
        {*/
            /*std::ifstream file("geode/temp/Ryzen/" + id + "/mod.json");
            std::string content((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));*/
            name = (jute::parser::parse(responseString)["name"].as_string());
            developer = (jute::parser::parse(responseString)["developer"].as_string());
            description = (jute::parser::parse(responseString)["description"].as_string());
        /*}
        else {
            addChild(CCLabelTTF::create(("Failed to download mod.json from \n" + responseString).c_str(), "arial", 8.f), 0, 521);
            name = "";
            developer = "";
            description = "";
        }*/
        setupModInfo(nullptr);
    }
    void getModLogo(CCHttpClient* client, CCHttpResponse* response) {
        menu->setTouchEnabled(1);
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
    void downloadMod(CCHttpClient* client, CCHttpResponse* response) {
        menu->setTouchEnabled(1);
        Ryzen_DownloadBtn_001->stopAllActions();
        Ryzen_DownloadBtn_001->runAction(CCFadeTo::create(0.1f, 90));
        //get response str
        std::vector<char>* responseData = response->getResponseData();
        std::string responseString(responseData->begin(), responseData->end());
        if (S_OK == URLDownloadToFile(NULL,
            responseString.c_str(),
            ("geode/mods/" + id + ".geode").c_str(),
            0, NULL))
        {
            ModUtils::log(("geode/mods/" + id + ".geode downloaded"));
            //Dont load mods
            CSimpleIni ini;
            ini.LoadFile("geode/config/Ryzen.ini");
            if (!ini.GetBoolValue("Ryzen", "Dont load mods", false)) {//so load mod
                int arg = 2;
                zip_extract(("geode/mods/" + id + ".geode").c_str(), ("geode/unzipped/" + id + "").c_str(), on_extract_entry, &arg);
            }
            //turn download btn to delete btn
            Ryzen_DownloadBtn_001->setNormalImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
            Ryzen_DownloadBtn_001->setSelectedImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
            Ryzen_DownloadBtn_001->setTarget(this, menu_selector(ModItem::deleteMe));
            Ryzen_DownloadBtn_001->runAction(CCFadeIn::create(0.1f));
        }
        else {
        }
    }
};

void RyzenLayer::getModsResponse(CCHttpClient* client, CCHttpResponse* response) {
    CCMenu_->setTouchEnabled(1);
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
        RyzenLayer::PageDown(0);
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
    CCHttpRequest_->setUrl(("https://user95401.undo.it/Ryzen/get.php?list&page=" + to_string(Page) + "&searhquery=" + url_encode(idInput->getString())).c_str());
    CCHttpRequest_->setResponseCallback(this, httpresponse_selector(RyzenLayer::getModsResponse));
    CCHttpClient::getInstance()->send(CCHttpRequest_);
    CCHttpRequest_->release();
    CCMenu_->setTouchEnabled(0);
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
    if (Page<2) return;//Page<1 0 -1 etcsddvf
    --Page;
    getMods(CCMenu_mods);
}

void RyzenLayer::UpdatePageLbl(CCObject* object) {
    PageLbl->setString((to_string(Page) + "th page").c_str());
}

void RyzenLayer::gotopage(CCObject* object) {
    Page = stoi(PageInput->getString());
    UpdatePageLbl(object);
    getMods(object);
}

void RyzenLayer::OpenUpSearchSetup(CCObject* object) {
    if (SearchLayer->numberOfRunningActions() > 0) return;
    if (SearchLayer->getChildByTag(1642)) {
        SearchLayer->removeChildByTag(1642);
        SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3f, CCPoint(SearchLayer->getPositionX(), 115.000f))));
        gj_findBtn->setNormalImage(ModUtils::createSprite("gj_findBtn_001.png"));
        gj_findBtn->setSelectedImage(gj_findBtn->getNormalImage());
    }
    else {
        SearchLayer->addChild(CCNode::create(), 1, 1642);
        SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3f, CCPoint(SearchLayer->getPositionX(), 0.000f))));
        gj_findBtn->setNormalImage(ModUtils::createSprite("gj_findBtnOff_001.png"));
        gj_findBtn->setSelectedImage(gj_findBtn->getNormalImage());
    }
}

void RyzenLayer::OpenUpPageSetup(CCObject* object) {
    if (PageSetupLayer->numberOfRunningActions() > 0) return;
    if (PageSetupLayer->getChildByTag(138)) {
        PageSetupLayer->removeChildByTag(138);
        PageSetupLayer->runAction(CCEaseBackInOut::create(CCMoveTo::create(0.3f, CCPoint(PageSetupLayer->getPositionX(), -120.f))));
        PageLbl->setOpacity(90);
    }
    else {
        PageSetupLayer->addChild(CCNode::create(), 1, 138);
        PageSetupLayer->runAction(CCEaseBackInOut::create(CCMoveTo::create(0.3f, CCPoint(PageSetupLayer->getPositionX(), 0.f))));
        PageLbl->setOpacity(180);
    }
}

void RyzenLayer::addMod(CCObject* object) {
    CCApplication::sharedApplication()->openURL("https://user95401.undo.it/Ryzen");
}

bool RyzenLayer::init() {
    setKeypadEnabled(true);
    setTouchEnabled(true);
    //PageSetupLayer
    if (PageSetupLayer = CCLayer::create()) {
        //add and setup
        addChild(PageSetupLayer, 20, 621);
        PageSetupLayer->runAction(CCEaseBackInOut::create(CCMoveTo::create(0.5f, CCPoint(PageSetupLayer->getPositionX(), -120.f))));
        //menu
        CCMenu* PageSetupLayer_Menu = CCMenu::create();
        PageSetupLayer_Menu->setPositionY(28.000f);
        PageSetupLayer->addChild(PageSetupLayer_Menu);
        //idInput
        PageInput = CCTextInputNode::create("", PageSetupLayer, "chatFont.fnt", 90.f, 20.f);//"developer.modname"
        PageInput->getTextField()->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
        PageInput->setAllowedChars("0123456789");
        PageSetupLayer_Menu->addChild(PageInput);
        //PageInputbtn1
        auto PageInputbtn1Sprite = ButtonSprite::create("Hop to", 0, false, "goldFont.fnt", "GJ_button_05.png", 0, 0.6);
        auto PageInputbtn1 = CCMenuItemSpriteExtra::create(PageInputbtn1Sprite, this, menu_selector(RyzenLayer::gotopage));
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
    //search layer
    if (SearchLayer = CCLayer::create()) {
        //add and setup
        addChild(SearchLayer, 20, 3206);
        SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.5f, CCPoint(SearchLayer->getPositionX(), 115.000f))));
        //menu
        CCMenu* SearchLayer_Menu = CCMenu::create();
        SearchLayer_Menu->setPositionY(CCDirector::sharedDirector()->getScreenTop()-51);
        SearchLayer->addChild(SearchLayer_Menu);
        //SquareShadowCorner
        CCSprite* SearchLayerShadowCorner = ModUtils::createSprite("groundSquareShadow_001.png");
        SearchLayerShadowCorner->setScaleX(0.900f);
        SearchLayerShadowCorner->setScaleY(5.000f);
        SearchLayerShadowCorner->setRotation(90.000f);
        SearchLayer_Menu->addChild(SearchLayerShadowCorner, -8);
        //square01_001
        CCScale9Sprite* CCScale9Sprite_ = CCScale9Sprite::create("square01_001.png");
        CCScale9Sprite_->setContentSize({ 416.000f, 2300.000f });
        CCScale9Sprite_->setAnchorPoint({ 0.5f, 0.0f });
        SearchLayer_Menu->addChild(CCScale9Sprite_, -5);
        //idInput
        idInput = CCTextInputNode::create("", SearchLayer, "chatFont.fnt", 290.f, 20.f);//"developer.modname"
        idInput->setAllowedChars(" !\"#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
        idInput->setPosition({ -40.000f, 30.000f });
        SearchLayer_Menu->addChild(idInput);
        //inputbg
        CCScale9Sprite* idInputBG = CCScale9Sprite::create("square02_001.png");
        idInputBG->setContentSize({ idInput->getContentSize().width * 2 + 20, idInput->getContentSize().height * 2 + 15 });
        idInputBG->setScale(0.5f);
        idInputBG->setOpacity(60);
        idInputBG->setPosition(idInput->getPosition());
        SearchLayer_Menu->addChild(idInputBG, -1);
        auto SearchLayerbtn1Sprite = ButtonSprite::create("Reload", 0, false, "goldFont.fnt", "GJ_button_05.png", 0, 0.6);
        auto SearchLayerbtn1 = CCMenuItemSpriteExtra::create(SearchLayerbtn1Sprite, this, menu_selector(RyzenLayer::getMods));
        SearchLayerbtn1->setPositionY(30.000f);
        SearchLayerbtn1->setPositionX(158.000f);
        SearchLayerbtn1->CCMenuItemSpriteExtra::setScale(0.900f);
        SearchLayer_Menu->addChild(SearchLayerbtn1);
    }
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
    SquareShadowCorner1->setScaleX(0.10);
    SquareShadowCorner1->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner1->getContentSize().height);
    SquareShadowCorner1->setAnchorPoint({ 0, 0 });
    addChild(SquareShadowCorner1, 10);
    //SquareShadowCorner2
    CCSprite* SquareShadowCorner2 = ModUtils::createSprite("groundSquareShadow_001.png");
    SquareShadowCorner2->setScaleX(-0.10);
    SquareShadowCorner2->setScaleY(CCDirector::sharedDirector()->getWinSize().height / SquareShadowCorner2->getContentSize().height);
    SquareShadowCorner2->setAnchorPoint({ 0, 0 });
    SquareShadowCorner2->setPositionX(CCDirector::sharedDirector()->getScreenRight());
    addChild(SquareShadowCorner2, 10);
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
    PageLbl = CCMenuItemLabel::create(CCLabelTTF::create((to_string(Page) + "th page").c_str(), "Arial", 10.f), this, menu_selector(RyzenLayer::OpenUpPageSetup));
    PageLbl->setAnchorPoint({ 0.5f, -0.15f });
    PageLbl->setPositionY(CCDirector::sharedDirector()->getWinSize().height / -2);
    PageLbl->setOpacity(90);
    CCMenu_->addChild(PageLbl);//add lbl to layer
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
    CCMenuItemSpriteExtra* Ryzen_ReloadBtn_001 = CCMenuItemSpriteExtra::create(ModUtils::createSprite("Ryzen_ReloadBtn_001.png", 4.0f), this, menu_selector(RyzenLayer::getMods));
    Ryzen_ReloadBtn_001->setPosition({
        (CCDirector::sharedDirector()->getWinSize().width / 2) - 35,
        (CCDirector::sharedDirector()->getWinSize().height / -2) + 35
        });
    Ryzen_ReloadBtn_001->CCMenuItemSpriteExtra::setScale(0.7f);
    CCMenu_->addChild(Ryzen_ReloadBtn_001);//add GJ_replayBtn_001
    //gj_findBtn
    gj_findBtn = CCMenuItemSpriteExtra::create(ModUtils::createSprite("gj_findBtn_001.png"), this, menu_selector(RyzenLayer::OpenUpSearchSetup));
    gj_findBtn->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 82.000f });
    CCMenu_->addChild(gj_findBtn);//add GJ_replayBtn_001
    //addmodbtn
    CCMenuItemSpriteExtra* GJ_plus3Btn_001 = CCMenuItemSpriteExtra::create(ModUtils::createSprite("GJ_plus3Btn_001.png"), this, menu_selector(RyzenLayer::addMod));
    GJ_plus3Btn_001->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 46.000f });
    GJ_plus3Btn_001->CCMenuItemSpriteExtra::setScale(1.700f);
    CCMenu_->addChild(GJ_plus3Btn_001);//add GJ_replayBtn_001
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
