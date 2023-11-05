#include "mod_utils.hpp"
#include "hooks.hpp"
#include "jute.h"
using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;
using namespace std;

#include "RyzenLayer.hpp"
#include "CocosHooks.hpp"

bool(__thiscall* geodemodslistinit)(CCLayer*);
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
    secretDoorBtn_open_001->setPosition({ CCDirector::sharedDirector()->getScreenRight() - 25.000f, 32.000f });
    self->addChild(CCMenu::createWithItem(secretDoorBtn_open_001), 100);
    reinterpret_cast<CCMenu*>(secretDoorBtn_open_001->getParent())->setPosition(CCPointZero);
    return true;
}

inline void(__thiscall* FLAlertLayer_show_0x23560)(FLAlertLayer* self);//0x23560
void __fastcall FLAlertLayer_show_0x23560_H(FLAlertLayer* self, void*) {
    if (popuptoreplace) self = popuptoreplace;
    FLAlertLayer_show_0x23560(self);
    popuptoreplace = nullptr;
}

class Ryzen {
public:
    static Ryzen* Start() {
        Ryzen* pRet = new Ryzen();
        ModUtils::log("Ryzen was loaded");
        if (pRet && pRet->init()) {
            return pRet;
        }
        else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }
    bool init() {
        UpdateConfig();
        CreateHooks();
        CSimpleIni ini;
        ini.LoadFile("geode/config/Ryzen.ini");
        if (!ini.GetBoolValue("Ryzen", "Disable auto updating", false)) UpdateMod();
        return true;
    }
    void UpdateConfig() {
        ModUtils::log(__func__);
        filesystem::create_directories("geode/config");
        CSimpleIni ini;
        ini.LoadFile("geode/config/Ryzen.ini");
        if (!(ini.KeyExists("Ryzen", "Auto calculation")))
            ini.SetBoolValue("Ryzen", "Dont load mods", false, "; Don't load downloaded mods without relaunch game");
        if (!(ini.KeyExists("Ryzen", "Disable auto updating")))
            ini.SetBoolValue("Ryzen", "Disable auto updating", false, "; Don't download actual version of Ryzen");
        ini.SaveFile("geode/config/Ryzen.ini");
    }
    static void CreateHooks() {
        ModUtils::log(__func__);
        MH_SafeInitialize();
        HOOK(base + 0x23560, FLAlertLayer_show_0x23560);
        CreateCocosHooks();
        //getGeodeInitAddr
        CCHttpRequest* CCHttpRequest_ = new CCHttpRequest;
        CCHttpRequest_->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        CCHttpRequest_->setUrl("https://user95401.undo.it/Ryzen/get.php?GeodeInitAddr");
        CCHttpRequest_->setResponseCallback(CCNode::create(), httpresponse_selector(Ryzen::CreateGeodeHooks));
        CCHttpClient::getInstance()->send(CCHttpRequest_);
        CCHttpRequest_->release();
    }
    void CreateGeodeHooks(CCHttpClient* client, CCHttpResponse* response) {
        ModUtils::log(__func__);
        std::vector<char>* responseData = response->getResponseData();
        std::string responseString(responseData->begin(), responseData->end());
        //respose tests
        int responseval;
        if (responseString == "" && stoi(responseString) < 11) {
            CCMessageBox(("Cant get ModListLayer::init address\nserver respose: "+ responseString).c_str(), __func__);
            responseval = 693696;//last addr
        }
        else responseval = stoi(responseString);
        //hook
        MH_SafeInitialize();
        HOOK((DWORD)GetModuleHandleA("Geode.dll") + responseval, geodemodslistinit);
    }
    void UpdateMod() {
        //getGeodeInitAddr
        CCHttpRequest* CCHttpRequest_ = new CCHttpRequest;
        CCHttpRequest_->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        CCHttpRequest_->setUrl("https://user95401.undo.it/Ryzen/get.php?UpdateMod");
        CCHttpRequest_->setResponseCallback(CCNode::create(), httpresponse_selector(Ryzen::UpdateModResponse));
        CCHttpClient::getInstance()->send(CCHttpRequest_);
        CCHttpRequest_->release();
    }
    void UpdateModResponse(CCHttpClient* client, CCHttpResponse* response) {
        std::vector<char>* responseData = response->getResponseData();
        std::string responseString(responseData->begin(), responseData->end());
        //respose tests
        if (responseString == "") return;
        std::vector<std::string> vect = ModUtils::explode(responseString, ',');
        ifstream in("geode/unzipped/user95401.Ryzen/mod.json");
        string str = "";
        string tmp;
        while (getline(in, tmp)) str += tmp;
        jute::jValue json = jute::parser::parse(str);
        string version = (json["version"].as_string());
        if (version == vect[0]) return ModUtils::log(("u have actual version"));
        //download
        if (MessageBox(nullptr, string("New version founded, download it?").c_str(),
            ("New Ryzen version - " + vect[0]).c_str(), MB_OKCANCEL | MB_ICONINFORMATION) != IDOK) return;
        if (S_OK == URLDownloadToFile(NULL,
            vect[1].c_str(),
            "geode/mods/user95401.Ryzen.geode",
            0, NULL))
        {
            ModUtils::log(("last version downloaded"));
            if (MessageBox(nullptr, string("Last version downloaded, close game \nto restart with new version?").c_str(),
                ("New Ryzen version - " + vect[0]).c_str(), MB_OKCANCEL | MB_ICONINFORMATION) == IDOK) {
                ExitProcess(-1);
            }
        }
        else ModUtils::log(
            ("cant download last version from "
                "\"" + responseString + "\""
                " to "
                "\"" "geode/mods/user95401.Ryzen.geode" "\""
                "!"));
    }
};

DWORD WINAPI PROCESS_ATTACH(void* hModule) {
    Ryzen::Start();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, PROCESS_ATTACH, hModule, 0, 0);
    return TRUE;
}
