#pragma once
#include "mod_utils.hpp"
#include "hooks.hpp"

class RyzenLayer : public cocos2d::CCLayer {
protected:
    cocos2d::CCMenu* CCMenu_mods;
    int Page;
    cocos2d::CCLabelTTF* PageLbl;
    cocos2d::CCMenu* CCMenu_;
    cocos2d::CCLayer* SearchLayer;
    cocos2d::CCSprite* SquareShadow;
    gd::CCMenuItemSpriteExtra* SearchLayer_leftBtn;
    gd::CCTextInputNode* idInput;
    virtual bool init();
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
    void getMods(cocos2d::CCObject*);
    void getModsResponse(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
    void PageUp(cocos2d::CCObject*);
    void PageDown(cocos2d::CCObject*);
    void UpdatePageLbl(cocos2d::CCObject*);
    void OpenUpSearchSetup(cocos2d::CCObject*);
public:
    static RyzenLayer* create();
    void openMe(cocos2d::CCObject* object);
};