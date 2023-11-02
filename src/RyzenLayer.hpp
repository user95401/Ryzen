#pragma once
#include "mod_utils.hpp"
#include "hooks.hpp"

class RyzenLayer : public cocos2d::CCLayer {
protected:

    cocos2d::CCMenu* CCMenu_;
    cocos2d::CCMenu* CCMenu_mods;

    //page
    int Page;
    cocos2d::CCMenuItemLabel* PageLbl;
    cocos2d::CCLayer* PageSetupLayer;
    gd::CCTextInputNode* PageInput;

    //search
    cocos2d::CCLayer* SearchLayer;
    gd::CCTextInputNode* idInput;
    gd::CCMenuItemSpriteExtra* gj_findBtn;

    virtual bool init();
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
    void getMods(cocos2d::CCObject*);
    void getModsResponse(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void PageUp(cocos2d::CCObject*);
    void PageDown(cocos2d::CCObject*);
    void UpdatePageLbl(cocos2d::CCObject*);
    void gotopage(cocos2d::CCObject*);
    void OpenUpPageSetup(cocos2d::CCObject*);
    void OpenUpSearchSetup(cocos2d::CCObject*);
    void addMod(cocos2d::CCObject*);

public:
    static RyzenLayer* create();
    void openMe(cocos2d::CCObject* object);
};