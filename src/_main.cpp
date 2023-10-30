#include "mod_utils.hpp"
#include "hooks.hpp"
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;

#include "RyzenLayer.hpp"

inline bool(__thiscall* geodemodslistinit)(CCLayer*);
bool __fastcall geodemodslistinit_H(CCLayer* self) {
    geodemodslistinit(self);
    twoTimesLayerInitHookEscape(self);//fucking works

    CCMenuItemSpriteExtra* secretDoorBtn_open_001 = CCMenuItemSpriteExtra::create(
        ModUtils::createSprite("secretDoorBtn_open_001.png"),
        self,
        menu_selector(RyzenLayer::openMe)
    );
    secretDoorBtn_open_001->m_bAnimationEnabled = false;
    secretDoorBtn_open_001->m_bColorEnabled = true;
    secretDoorBtn_open_001->setPosition({CCDirector::sharedDirector()->getScreenRight() - 35.f, 35.f});
    self->addChild(CCMenu::createWithItem(secretDoorBtn_open_001), 100);
    reinterpret_cast<CCMenu*>(secretDoorBtn_open_001->getParent())->setPosition(CCPointZero);

    return true;
}

DWORD WINAPI Ryzen(void* hModule) {
    MH_SafeInitialize();
    HOOK((DWORD)GetModuleHandleA("Geode.dll") + 0xA7E60, geodemodslistinit);
    ModUtils::log("Ryzen was loaded");
    return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, Ryzen, hModule, 0, 0);
    return TRUE;
}
