#pragma once
#include "mod_utils.hpp"
#include "hooks.hpp"

class RyzenLayer : public cocos2d::CCLayer {
protected:
    gd::GJListLayer* GJListLayer_;
    gd::CustomListView* CustomListView_;
    virtual bool init();
    virtual void keyBackClicked();
    void onBack(cocos2d::CCObject*);
public:
    static RyzenLayer* create();
    void openMe(cocos2d::CCObject* object);
};