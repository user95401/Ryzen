#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
using namespace geode::prelude;

class ModItem : public CCMenuItem {
public:
    matjson::Value json;
    std::string id;
    static ModItem* create(std::string sId) {
        auto pRet = new ModItem();
        if (pRet) {
            pRet->id = sId;
            pRet->init();
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
                menu->setID("menu");
                pRet->addChild(menu, 10);
                //Ryzen_InfoBtn_001
                CCMenuItemSpriteExtra* Ryzen_InfoBtn_001 = CCMenuItemSpriteExtra::create(CCSprite::create("Ryzen_InfoBtn_001.png"_spr), pRet, nullptr);
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
                //Notification
                auto ntfy = Notification::create("Loading...", NotificationIcon::Loading);
                dynamic_cast<CCScale9Sprite*>(ntfy->getChildren()->objectAtIndex(0))->setOpacity(0);
                ntfy->setID("ntfy");
                menu->addChild(ntfy);
            }
            pRet->loadMoreeeInfo();
            return pRet;
        }
        else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }
    void loadMoreeeInfo() {
        web::AsyncWebRequest().
            fetch(fmt::format("http://ryzen.user95401.undo.it/api/mod-json.php?id={}", this->id)).
            json().
            then(
                [this](matjson::Value const& data) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<ModItem*>(this)) {
                        this->json = data;
                        //InstalledMod
                        Mod* InstalledMod = nullptr;
                        if(Loader::get()->isModInstalled(this->json["id"].as_string())) 
                            InstalledMod = Loader::get()->getInstalledMod(this->json["id"].as_string());
                        //ntfy
                        auto ntfy = dynamic_cast<Notification*>(this->getChildByIDRecursive("ntfy"));
                        if (ntfy) ntfy->setVisible(0);
                        auto menu = dynamic_cast<CCMenu*>(this->getChildByIDRecursive("menu"));
                        //name
                        CCLabelTTF* name = CCLabelTTF::create(json["name"].as_string().c_str(), "arial", 12.f);
                        name->setHorizontalAlignment(kCCTextAlignmentLeft);
                        name->setAnchorPoint({ 0.0f, 0.5f });
                        name->setPositionX(-132.000f);
                        name->setPositionY(12.000f);
                        addChild(name);
                        //version
                        CCLabelTTF* version = CCLabelTTF::create(json["version"].as_string().c_str(), "arial", 8.f);
                            //change str if new ver
                        if (InstalledMod and InstalledMod->getVersion().toString() != json["version"].as_string()) {
                            version->setString(
                                (InstalledMod->getVersion().toString() + " >> " + json["version"].as_string()).c_str()
                            );
                        };/**/
                        version->setHorizontalAlignment(kCCTextAlignmentLeft);
                        version->setAnchorPoint({ -0.000f, 0.700f });
                        version->setOpacity(150);
                        version->setPositionX(name->getPositionX() + name->getContentSize().width + 3.f);
                        version->setPositionY(name->getPositionY());
                        addChild(version);
                        //developer
                        CCLabelTTF* developer = CCLabelTTF::create(("By: " + json["developer"].as_string()).c_str(), "arial", 8.f);
                        developer->setHorizontalAlignment(kCCTextAlignmentLeft);
                        developer->setAnchorPoint({ 0.0f, 0.5f });
                        developer->setPositionX(-132.000f);
                        developer->setPositionY(0.000f);
                        addChild(developer);
                        CCLabelTTF* description = CCLabelTTF::create(json["description"].as_string().c_str(), "arial", 6.f);
                        description->setHorizontalAlignment(kCCTextAlignmentLeft);
                        description->setAnchorPoint({ 0.0f, 0.5f });
                        description->setPositionX(-132.000f);
                        description->setPositionY(-12.000f);
                        addChild(description);
                        //logo
                        auto logoBtn = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png"), CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png"), this, nullptr);
                        logoBtn->setPositionX(-155.000f);
                        logoBtn->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.3, 90), CCFadeTo::create(0.3, 160), nullptr)));
                        logoBtn->setID("logoBtn");
                        loadLogo();
                        addChild(logoBtn);
                        //Ryzen_DownloadBtn_001
                        //auto download = CCMenuItemSpriteExtra::create(CCSprite::create("Ryzen_DownloadBtn_001.png"_spr), this, menu_selector(ModItem::onDownloadMod));
                        //download->setPositionX(155.000f);
                        //menu->addChild(download);
                        //deletebtn if exists
                        /*if (
                        CCFileUtils::sharedFileUtils()->isFileExist(("geode/mods/" + id + ".geode").c_str())
                        &&
                        localjson["version"].as_string() == json["version"].as_string()
                        ) {
                        Ryzen_DownloadBtn_001->setNormalImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
                        Ryzen_DownloadBtn_001->setSelectedImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
                        Ryzen_DownloadBtn_001->setTarget(this, menu_selector(ModItem::deleteMe));
                        }*/
                        //add spr to btn if new ver
                        /*if (
                        localjson["version"].as_string() != string("")
                        &&
                        localjson["version"].as_string() != json["version"].as_string()
                        ) {
                        CCSprite* edit_upBtn2_001 = ModUtils::createSprite("edit_upBtn2_001.png");
                        edit_upBtn2_001->setAnchorPoint({ -1.250f, -1.250f });
                        edit_upBtn2_001->setScale(0.500f);
                        Ryzen_DownloadBtn_001->addChild(edit_upBtn2_001, 5, 857);
                        }*/
                    }
                }
            ).
            expect(
                [this](std::string const& error) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<ModItem*>(this)) {
                        auto ntfy = dynamic_cast<Notification*>(this->getChildByIDRecursive("ntfy"));
                        auto ntfybg = dynamic_cast<CCScale9Sprite*>(ntfy->getChildren()->objectAtIndex(0));
                        if (ntfy) ntfy->setString(error.data());
                        if (ntfy) ntfy->setIcon(NotificationIcon::Error);
                        if (ntfybg) ntfy->setScale((this->getContentSize().width - 22) / ntfybg->getContentSize().width);
                    }
                }
            );
    }
    void loadLogo() {
        web::AsyncWebRequest()
            .fetch(fmt::format("http://ryzen.user95401.undo.it/api/logo.php?id={}", this->id))
            .bytes()
            .then([this](ByteVector& catgirl) {
            //yaah, safe asf brrrooo
            if (typeinfo_cast<ModItem*>(this)) {
                // print response
                auto path = Mod::get()->getTempDir() / "mods" / this->id / "logo.png";
                ghc::filesystem::create_directories(path.parent_path());
                std::ofstream outfile(path, std::ios::binary);
                for (auto atom : catgirl) {
                    outfile << atom;
                }
                outfile.close();
                //create spr and append to btn
                auto logoBtn = dynamic_cast<CCMenuItemSprite*>(this->getChildByIDRecursive("logoBtn"));
                CCSprite* logo = CCSprite::create(path.string().c_str());//sprite
                if (logo->getContentSize().width > logo->getContentSize().height)
                    logo->setScale(logoBtn->getContentSize().width / logo->getContentSize().width);
                else logo->setScale(logoBtn->getContentSize().height / logo->getContentSize().height);
                logo->setPosition({ logoBtn->getContentSize().width / 2, logoBtn->getContentSize().height / 2 });
                /*logoBtn->addChild(logo);
                CCSprite* hiddenNode = dynamic_cast<CCSprite*>(logoBtn->getNormalImage());
                hiddenNode->runAction(CCRepeatForever::create(CCFadeOut::create(0.f)));*/
                logoBtn->setVisible(0);
                logo->setPosition(logoBtn->getPosition());
                logoBtn->getParent()->addChild(logo);
            };
                })
            .expect([this](std::string const& error) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<ModItem*>(this)) {};
                });
    }
    void gonDownloadMod(CCObject*) {
        auto json = web::fetchJSON(fmt::format("http://ryzen.user95401.undo.it/api/download.php?id={}", this->id));
        auto pop = FLAlertLayer::create(
            nullptr,
            "Download it...",
            fmt::format(
                "Download the" "\n" 
                "<cj>{}</c>" "\n"
                "into <cy>{}</c>?" "\n"
                "<co>{}</c>",
                json.value()["link"].as_string(),
                json.value()["path"].as_string(),
                json.value()["opinion"].as_string()
            ),
            "Close",
            "Yes",
            420.f
        );
        //dynamic_cast<CCMenuItemSpriteExtra*>(pop->m_button2->getParent())->setTarget(this, menu_selector(ModItem::downloadMod));
        pop->show();
    }
    void gdownloadMod(CCObject* btn) {
        auto btn2 = dynamic_cast<CCMenuItemSpriteExtra*>(btn);
        btn2->getParent()->setVisible(0);//hide menu
        //Notification
        auto ntfy = Notification::create("Downloading...", NotificationIcon::Loading);
        dynamic_cast<CCScale9Sprite*>(ntfy->getChildren()->objectAtIndex(0))->setOpacity(60);
        ntfy->setID("ntfy");
        ntfy->setPosition(btn2->getParent()->getPosition());
        btn2->getParent()->getParent()->addChild(ntfy, 3);
        auto json = web::fetchJSON(fmt::format("http://ryzen.user95401.undo.it/api/download.php?id={}", this->id));
        web::AsyncWebRequest()
            .fetch(json.value()["link"].as_string())
            .bytes()
            .then([this, json, ntfy](ByteVector& catgirl) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<ModItem*>(this)) {
                        auto linkAsPath = ghc::filesystem::path(json.value()["link"].as_string());
                        auto path = geode::dirs::getGameDir() / json.value()["path"].as_string() / linkAsPath.filename();
                        ghc::filesystem::create_directories(path.parent_path());
                        std::ofstream outfile(path, std::ios::binary);
                        //slider->m_height = catgirl.size();
                        for (auto atom : catgirl) {
                            outfile << atom;
                        }
                        outfile.close();
                        ntfy->getParent()->getParent()->removeFromParent();
                        Notification::create(fmt::format("Downloaded {}.", this->id), NotificationIcon::Success)->show();
                    };
                })
            .expect([this, ntfy](std::string const& error) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<ModItem*>(this)) {
                        ntfy->getParent()->getParent()->removeFromParent();
                        Notification::create(fmt::format("Downloading of {} failed!", this->id), NotificationIcon::Error)->show();
                    };
                });
    };
};
class PackItem : public CCMenuItem {
public:
    matjson::Value json;
    std::string id;
    static PackItem* create(std::string sId) {
        auto pRet = new PackItem();
        if (pRet) {
            pRet->id = sId;
            pRet->init();
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
                menu->setID("menu");
                pRet->addChild(menu, 10);
                //Ryzen_InfoBtn_001
                CCMenuItemSpriteExtra* Ryzen_InfoBtn_001 = CCMenuItemSpriteExtra::create(CCSprite::create("Ryzen_InfoBtn_001.png"_spr), pRet, nullptr);
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
                //Notification
                auto ntfy = Notification::create("Loading...", NotificationIcon::Loading);
                dynamic_cast<CCScale9Sprite*>(ntfy->getChildren()->objectAtIndex(0))->setOpacity(0);
                ntfy->setID("ntfy");
                menu->addChild(ntfy);
            }
            //pRet->loadMoreeeInfo();
            return pRet;
        }
        else {
            delete pRet;
            pRet = NULL;
            return NULL;
        }
    }
    void gloadMoreeeInfo() {
        web::AsyncWebRequest().
            fetch(fmt::format("http://ryzen.user95401.undo.it/api/json.php?id={}", this->id)).
            json().
            then(
                [this](matjson::Value const& data) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<PackItem*>(this)) {
                        this->json = data;
                        auto ntfy = dynamic_cast<Notification*>(this->getChildByIDRecursive("ntfy"));
                        if (ntfy) ntfy->setVisible(0);
                        auto menu = dynamic_cast<CCMenu*>(this->getChildByIDRecursive("menu"));
                        CCLabelTTF* name = CCLabelTTF::create(json["name"].as_string().c_str(), "arial", 12.f);
                        name->setHorizontalAlignment(kCCTextAlignmentLeft);
                        name->setAnchorPoint({ 0.0f, 0.5f });
                        name->setPositionX(-132.000f);
                        name->setPositionY(12.000f);
                        addChild(name);
                        CCLabelTTF* version = CCLabelTTF::create(json["version"].as_string().c_str(), "arial", 8.f);
                        //change str if new ver
                        /*if (
                        localjson["version"].as_string() != string("")
                        &&
                        localjson["version"].as_string() != json["version"].as_string()
                        ) {
                        CCLabelTTFversion->setString(
                        (localjson["version"].as_string() + " >> " +
                        json["version"].as_string()).c_str());
                        }*/
                        version->setHorizontalAlignment(kCCTextAlignmentLeft);
                        version->setAnchorPoint({ -0.000f, 0.700f });
                        version->setOpacity(150);
                        version->setPositionX(name->getPositionX() + name->getContentSize().width + 3.f);
                        version->setPositionY(name->getPositionY());
                        addChild(version);
                        CCLabelTTF* developer = CCLabelTTF::create(("By: " + json["developer"].as_string()).c_str(), "arial", 8.f);
                        developer->setHorizontalAlignment(kCCTextAlignmentLeft);
                        developer->setAnchorPoint({ 0.0f, 0.5f });
                        developer->setPositionX(-132.000f);
                        developer->setPositionY(0.000f);
                        addChild(developer);
                        //logo
                        auto logoBtn = CCMenuItemSprite::create(CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png"), CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png"), this, nullptr);
                        logoBtn->setPositionX(-155.000f);
                        logoBtn->runAction(CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.3, 90), CCFadeTo::create(0.3, 160), nullptr)));
                        logoBtn->setID("logoBtn");
                        //loadLogo();
                        addChild(logoBtn);
                        //Ryzen_DownloadBtn_001
                        //auto download = CCMenuItemSpriteExtra::create(CCSprite::create("Ryzen_DownloadBtn_001.png"_spr), this, menu_selector(ModItem::onDownloadMod));
                        //download->setPositionX(155.000f);
                        //menu->addChild(download);
                        //deletebtn if exists
                        /*if (
                        CCFileUtils::sharedFileUtils()->isFileExist(("geode/mods/" + id + ".geode").c_str())
                        &&
                        localjson["version"].as_string() == json["version"].as_string()
                        ) {
                        Ryzen_DownloadBtn_001->setNormalImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
                        Ryzen_DownloadBtn_001->setSelectedImage(ModUtils::createSprite("edit_delBtnSmall_001.png"));
                        Ryzen_DownloadBtn_001->setTarget(this, menu_selector(ModItem::deleteMe));
                        }*/
                        //add spr to btn if new ver
                        /*if (
                        localjson["version"].as_string() != string("")
                        &&
                        localjson["version"].as_string() != json["version"].as_string()
                        ) {
                        CCSprite* edit_upBtn2_001 = ModUtils::createSprite("edit_upBtn2_001.png");
                        edit_upBtn2_001->setAnchorPoint({ -1.250f, -1.250f });
                        edit_upBtn2_001->setScale(0.500f);
                        Ryzen_DownloadBtn_001->addChild(edit_upBtn2_001, 5, 857);
                        }*/
                    }
                }
            ).
            expect(
                [this](std::string const& error) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<ModItem*>(this)) {
                        auto ntfy = dynamic_cast<Notification*>(this->getChildByIDRecursive("ntfy"));
                        auto ntfybg = dynamic_cast<CCScale9Sprite*>(ntfy->getChildren()->objectAtIndex(0));
                        if (ntfy) ntfy->setString(error.data());
                        if (ntfy) ntfy->setIcon(NotificationIcon::Error);
                        if (ntfybg) ntfy->setScale((this->getContentSize().width - 22) / ntfybg->getContentSize().width);
                    }
                }
                );
    }
    void gloadLogo() {
        web::AsyncWebRequest()
            .fetch(fmt::format("http://ryzen.user95401.undo.it/api/logo.php?id={}", this->id))
            .bytes()
            .then([this](ByteVector& catgirl) {
            //yaah, safe asf brrrooo
            if (typeinfo_cast<PackItem*>(this)) {
                // print response
                auto path = Mod::get()->getTempDir() / "mods" / this->id / "logo.png";
                ghc::filesystem::create_directories(path.parent_path());
                std::ofstream outfile(path, std::ios::binary);
                for (auto atom : catgirl) {
                    outfile << atom;
                }
                outfile.close();
                //create spr and append to btn
                auto logoBtn = dynamic_cast<CCMenuItemSprite*>(this->getChildByIDRecursive("logoBtn"));
                CCSprite* logo = CCSprite::create(path.string().c_str());//sprite
                if (logo->getContentSize().width > logo->getContentSize().height)
                    logo->setScale(logoBtn->getContentSize().width / logo->getContentSize().width);
                else logo->setScale(logoBtn->getContentSize().height / logo->getContentSize().height);
                logo->setPosition({ logoBtn->getContentSize().width / 2, logoBtn->getContentSize().height / 2 });
                /*logoBtn->addChild(logo);
                CCSprite* hiddenNode = dynamic_cast<CCSprite*>(logoBtn->getNormalImage());
                hiddenNode->runAction(CCRepeatForever::create(CCFadeOut::create(0.f)));*/
                logoBtn->setVisible(0);
                logo->setPosition(logoBtn->getPosition());
                logoBtn->getParent()->addChild(logo);
            };
                })
            .expect([this](std::string const& error) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<PackItem*>(this)) {};
                });
    }
    void gonDownloadMod(CCObject*) {
        auto json = web::fetchJSON(fmt::format("http://ryzen.user95401.undo.it/api/download.php?id={}", this->id));
        auto pop = FLAlertLayer::create(
            nullptr,
            "Download it...",
            fmt::format(
                "Download the" "\n"
                "<cj>{}</c>" "\n"
                "into <cy>{}</c>?" "\n"
                "<co>{}</c>",
                json.value()["link"].as_string(),
                json.value()["path"].as_string(),
                json.value()["opinion"].as_string()
            ),
            "Close",
            "Yes",
            420.f
        );
        //dynamic_cast<CCMenuItemSpriteExtra*>(pop->m_button2->getParent())->setTarget(this, menu_selector(PackItem::downloadPack));
        pop->show();
    }
    void gdownloadPack(CCObject* btn) {
        auto btn2 = dynamic_cast<CCMenuItemSpriteExtra*>(btn);
        btn2->getParent()->setVisible(0);//hide menu
        //Notification
        auto ntfy = Notification::create("Downloading...", NotificationIcon::Loading);
        dynamic_cast<CCScale9Sprite*>(ntfy->getChildren()->objectAtIndex(0))->setOpacity(60);
        ntfy->setID("ntfy");
        ntfy->setPosition(btn2->getParent()->getPosition());
        btn2->getParent()->getParent()->addChild(ntfy, 3);
        auto json = web::fetchJSON(fmt::format("http://ryzen.user95401.undo.it/api/download.php?id={}", this->id));
        web::AsyncWebRequest()
            .fetch(json.value()["link"].as_string())
            .bytes()
            .then([this, json, ntfy](ByteVector& catgirl) {
            //yaah, safe asf brrrooo
            if (typeinfo_cast<PackItem*>(this)) {
                auto linkAsPath = ghc::filesystem::path(json.value()["link"].as_string());
                auto path = geode::dirs::getGameDir() / json.value()["path"].as_string() / linkAsPath.filename();
                ghc::filesystem::create_directories(path.parent_path());
                std::ofstream outfile(path, std::ios::binary);
                //slider->m_height = catgirl.size();
                for (auto atom : catgirl) {
                    outfile << atom;
                }
                outfile.close();
                ntfy->getParent()->getParent()->removeFromParent();
                Notification::create(fmt::format("Downloaded {}.", this->id), NotificationIcon::Success)->show();
            };
                })
            .expect([this, ntfy](std::string const& error) {
                    //yaah, safe asf brrrooo
                    if (typeinfo_cast<PackItem*>(this)) {
                        ntfy->getParent()->getParent()->removeFromParent();
                        Notification::create(fmt::format("Downloading of {} failed!", this->id), NotificationIcon::Error)->show();
                    };
                });
    };
};
class RyzenLayer : public CCLayer {
public:
    int p = 0;
    std::string q;
    void sendBtnFunc(std::string id) {
        auto BtnCmd = CCNode::create();
        BtnCmd->setID(id);
        return this->onBtn(BtnCmd);
    }
    void onBtn(CCObject* pCCObject) {
        auto what = dynamic_cast<CCNode*>(pCCObject);
        if (not what) return;
        //CCMessageBox(what->getID().c_str(), __FUNCTION__);
        if (what->getID() == "reload") reload();
        if (what->getID() == "add_link");
        if (what->getID() == "Search") {
            auto idInput = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("idInput"));
            q = idInput->getString();
            reload();
        };
        if (what->getID() == "OpenSearchBar") {
            auto gj_findBtn = typeinfo_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("OpenSearchBar"));
            auto SearchLayer = typeinfo_cast<CCLayer*>(this->getChildByIDRecursive("SearchLayer"));
            if (not gj_findBtn) return;
            if (not SearchLayer) return;
            if (SearchLayer->numberOfRunningActions() > 0) return;
            if (SearchLayer->getChildByTag(1642)) {
                SearchLayer->removeChildByTag(1642);
                SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3f, CCPoint(SearchLayer->getPositionX(), 115.000f))));
                gj_findBtn->setNormalImage(CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"));
                gj_findBtn->setSelectedImage(gj_findBtn->getNormalImage());
            }
            else {
                SearchLayer->addChild(CCNode::create(), 1, 1642);
                SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.3f, CCPoint(SearchLayer->getPositionX(), 0.000f))));
                gj_findBtn->setNormalImage(CCSprite::createWithSpriteFrameName("gj_findBtnOff_001.png"));
                gj_findBtn->setSelectedImage(gj_findBtn->getNormalImage());
            }
        }
        if (what->getID() == "OpenUpPageSetup") {
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            auto PageSetupLayer = typeinfo_cast<CCLayer*>(this->getChildByIDRecursive("PageSetupLayer"));
            if (not PageLbl) return;
            if (not PageSetupLayer) return;
            if (PageSetupLayer->numberOfRunningActions() > 0) return;
            if (PageSetupLayer->getChildByTag(138)) {
                PageSetupLayer->removeChildByTag(138);
                PageSetupLayer->runAction(CCEaseBackIn::create(CCMoveTo::create(0.3f, CCPoint(PageSetupLayer->getPositionX(), -120.f))));
                PageLbl->setOpacity(90);
            }
            else {
                PageSetupLayer->addChild(CCNode::create(), 1, 138);
                PageSetupLayer->runAction(CCEaseBackOut::create(CCMoveTo::create(0.3f, CCPoint(PageSetupLayer->getPositionX(), 0.f))));
                PageLbl->setOpacity(180);
            }
        }
        if (what->getID() == "gotopage") {
            auto PageInput = typeinfo_cast<CCTextInputNode*>(this->getChildByIDRecursive("PageInput"));
            if (not PageInput) return;
            p = (PageInput->getString().c_str() == "") ? 1 : atoi(PageInput->getString().data());
            p = p > 0 ? p : 0;
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            if (PageLbl) PageLbl->setString(fmt::format("{}th page", p).c_str());
            reload();
        };
        if (what->getID() == "PageDown") {
            --p;
            p = p > 0 ? p : 0;
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            if (PageLbl) PageLbl->setString(fmt::format("{}th page", p).c_str());
            reload();
        }
        if (what->getID() == "PageUp") {
            ++p;
            p = p > 0 ? p : 0;
            auto PageLbl = typeinfo_cast<CCLabelTTF*>(this->getChildByIDRecursive("PageLbl"));
            if (PageLbl) PageLbl->setString(fmt::format("{}th page", p).c_str());
            reload();
        }
        if (what->getID() == "switch") {
            auto modsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("modsMenu"));
            auto packsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("packsMenu"));
            modsMenu->setVisible(not modsMenu->isVisible());
            packsMenu->setVisible(not packsMenu->isVisible());
            reload();
        }
    }
    void reload() {
        auto modsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("modsMenu"));
        auto packsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("packsMenu"));
        if (modsMenu->isVisible()) loadMods();
        if (packsMenu->isVisible()) loadPacks(); 
    }
    void loadPacks() {
        auto packsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("packsMenu"));
        if (!packsMenu->getChildByTag(580)) packsMenu->addChild(
            Notification::create("Loading...", NotificationIcon::Loading), 1, 580
        );
        web::AsyncWebRequest()
            .fetch(fmt::format("http://ryzen.user95401.undo.it/api/packs-list.php?p={}&q={}", p, q))
            .text()
            .then([this, packsMenu](std::string const& data)
                {
                    if (data.empty()) {
                        if (p > 0)
                            this->sendBtnFunc("PageDown");
                        if (!packsMenu->getChildByTag(521)) packsMenu->addChild(
                            Notification::create("Packs wasnt founded...", NotificationIcon::Error), 1, 521
                        );
                    }
                    else {
                        // parse resulting data :3
                        std::string tmp;
                        std::stringstream ss(data);
                        std::vector<std::string> mods;
                        while (std::getline(ss, tmp, ',')) {
                            mods.push_back(tmp);
                        }
                        packsMenu->removeAllChildren();
                        for (auto mod : mods) {
                            auto entry = PackItem::create(mod);
                            if (rand() % 2 == 0)
                                entry->runAction(CCSequence::create(
                                    CCMoveBy::create(0.0f, CCPoint(500, 0)),
                                    CCEaseExponentialOut::create(CCMoveBy::create(0.5f, CCPoint(-500, 0))),
                                    nullptr
                                ));
                            else
                                entry->runAction(CCSequence::create(
                                    CCMoveBy::create(0.0f, CCPoint(-500, 0)),
                                    CCEaseExponentialOut::create(CCMoveBy::create(0.5f, CCPoint(500, 0))),
                                    nullptr
                                ));
                            packsMenu->addChild(entry);
                        }
                        packsMenu->alignItemsVerticallyWithPadding(10.0f);
                    };
                }
            )
            .expect([packsMenu](std::string const& error) {
                    /*if (!packsMenu->getChildByTag(41)) packsMenu->addChild(
                        Notification::create(error, NotificationIcon::Error), 1, 41
                    );*/
                });
    };
    void loadMods() {
        auto modsMenu = typeinfo_cast<CCMenu*>(this->getChildByID("modsMenu"));
        if (!modsMenu->getChildByTag(580)) modsMenu->addChild(
            Notification::create("Loading...", NotificationIcon::Loading), 1, 580
        );
        web::AsyncWebRequest()
            .fetch(fmt::format("http://ryzen.user95401.undo.it/api/mods-list.php?p={}&q={}", p, q))
            .text()
            .then([this, modsMenu](std::string const& data)
                {
                    if (data.empty()) {
                        if (p > 0) 
                            this->sendBtnFunc("PageDown");
                        if (!modsMenu->getChildByTag(521)) modsMenu->addChild(
                            Notification::create("Mods wasnt founded...", NotificationIcon::Error), 1, 521
                        );
                    }
                    else {
                        // parse resulting data :3
                        std::string tmp;
                        std::stringstream ss(data);
                        std::vector<std::string> mods;
                        while (std::getline(ss, tmp, ',')) {
                            mods.push_back(tmp);
                        }
                        modsMenu->removeAllChildrenWithCleanup(1);
                        for (auto mod : mods) {
                            auto entry = ModItem::create(mod);
                            if (rand() % 2 == 0)
                                entry->runAction(CCSequence::create(
                                    CCMoveBy::create(0.0f, CCPoint(500, 0)),
                                    CCEaseExponentialOut::create(CCMoveBy::create(0.5f, CCPoint(-500, 0))),
                                    nullptr
                                ));
                            else
                                entry->runAction(CCSequence::create(
                                    CCMoveBy::create(0.0f, CCPoint(-500, 0)),
                                    CCEaseExponentialOut::create(CCMoveBy::create(0.5f, CCPoint(500, 0))),
                                    nullptr
                                ));
                            modsMenu->addChild(entry);
                        }
                        modsMenu->alignItemsVerticallyWithPadding(10.0f);
                    };
                }
            )
            .expect([modsMenu](std::string const& error) {
                    /*if (!modsMenu->getChildByTag(41)) modsMenu->addChild(
                        Notification::create(error, NotificationIcon::Error), 1, 41
                    );*/
                });
    };
    static RyzenLayer* create() {
        auto ret = new RyzenLayer();
        if (ret) {
            ret->autorelease();
            ret->init();
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
                //packsMenu
                auto packsMenu = CCMenu::create();
                packsMenu->setID("packsMenu");
                packsMenu->setVisible(0);
                ret->addChild(packsMenu);
                //reloads
                ret->reload();
                //menu
                auto menu = CCMenu::create();
                ret->addChild(menu);
                //toprightbar
                {
                    //reloadmodsbtn
                    CCMenuItemSpriteExtra* Ryzen_ReloadBtn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::create("Ryzen_ReloadBtn_001.png"_spr),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    Ryzen_ReloadBtn_001->setID("reload");
                    Ryzen_ReloadBtn_001->setPosition({
                        (CCDirector::sharedDirector()->getWinSize().width / 2) - 35,
                        (CCDirector::sharedDirector()->getWinSize().height / -2) + 35
                        });
                    Ryzen_ReloadBtn_001->getNormalImage()->setScale(0.7f);
                    menu->addChild(Ryzen_ReloadBtn_001);//add GJ_replayBtn_001
                    //gj_findBtn
                    auto gj_findBtn = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    gj_findBtn->setID("OpenSearchBar");
                    gj_findBtn->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 82.000f });
                    menu->addChild(gj_findBtn);//add GJ_replayBtn_001
                    //addmodbtn
                    CCMenuItemSpriteExtra* GJ_plus3Btn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("GJ_plus3Btn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    GJ_plus3Btn_001->setID("add_link");
                    GJ_plus3Btn_001->setPosition({ (CCDirector::sharedDirector()->getWinSize().width / 2) - 38, 46.000f });
                    GJ_plus3Btn_001->getNormalImage()->setScale(1.700f);
                    menu->addChild(GJ_plus3Btn_001);//add GJ_replayBtn_001
                };
                //pagination
                {
                    //pagebtns
                    CCMenu* CCMenuPage = CCMenu::create();
                    CCMenuPage->setID("CCMenuPage");
                    ret->addChild(CCMenuPage);//add menu to layer
                    CCMenuItemSpriteExtra* edit_leftBtn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    edit_leftBtn_001->setID("PageDown");
                    CCMenuPage->addChild(edit_leftBtn_001);//add edit_rightBtn_001
                    CCMenuItemSpriteExtra* edit_rightBtn_001 = CCMenuItemSpriteExtra::create(
                        CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png"),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    edit_rightBtn_001->setID("PageUp");
                    CCMenuPage->addChild(edit_rightBtn_001);//add edit_rightBtn_001
                    CCMenuPage->alignItemsHorizontallyWithPadding(360.f + 20);
                    //PageLbl
                    auto PageLbl = CCMenuItemLabel::create(
                        CCLabelTTF::create(fmt::format("{}th page", ret->p).c_str(), "Arial", 10.f),
                        ret, menu_selector(RyzenLayer::onBtn)
                    );
                    PageLbl->setID("OpenUpPageSetup");
                    PageLbl->getLabel()->setID("PageLbl");
                    PageLbl->setAnchorPoint({ 0.5f, -0.15f });
                    PageLbl->setPositionY(CCDirector::sharedDirector()->getWinSize().height / -2);
                    PageLbl->setOpacity(90);
                    CCMenuPage->addChild(PageLbl);//add lbl to menu
                };
                //layers
                {
                    //PageSetupLayer
                    auto PageSetupLayer = CCLayer::create();
                    if (PageSetupLayer->init()) {
                        //add and setup
                        ret->addChild(PageSetupLayer, 20, 621);
                        PageSetupLayer->setID("PageSetupLayer");
                        PageSetupLayer->runAction(CCEaseBackInOut::create(CCMoveTo::create(0.5f, CCPoint(PageSetupLayer->getPositionX(), -120.f))));
                        //menu
                        CCMenu* PageSetupLayer_Menu = CCMenu::create();
                        PageSetupLayer_Menu->setPositionY(28.000f);
                        PageSetupLayer->addChild(PageSetupLayer_Menu);
                        //idInput
                        auto PageInput = CCTextInputNode::create(90.f, 20.f, "", 12, "chatFont.fnt");//"developer.modname"
                        PageInput->m_textField->setHorizontalAlignment(CCTextAlignment::kCCTextAlignmentLeft);
                        PageInput->setAllowedChars("0123456789");
                        PageInput->setID("PageInput");
                        PageSetupLayer_Menu->addChild(PageInput);
                        //PageInputbtn1
                        auto PageInputbtn1Sprite = ButtonSprite::create("Hop to", 0, false, "goldFont.fnt", "GJ_button_05.png", 0, 0.6);
                        auto PageInputbtn1 = CCMenuItemSpriteExtra::create(PageInputbtn1Sprite, ret, menu_selector(RyzenLayer::onBtn));
                        PageInputbtn1->setID("gotopage");
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
                    auto SearchLayer = CCLayer::create();
                    if (SearchLayer->init()) {
                        //add and setup
                        ret->addChild(SearchLayer, 20, 3206);
                        SearchLayer->setID("SearchLayer");
                        SearchLayer->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.5f, CCPoint(SearchLayer->getPositionX(), 115.000f))));
                        //menu
                        CCMenu* SearchLayer_Menu = CCMenu::create();
                        SearchLayer_Menu->setPositionY(CCDirector::sharedDirector()->getScreenTop() - 51);
                        SearchLayer->addChild(SearchLayer_Menu);
                        //SquareShadowCorner
                        CCSprite* SearchLayerShadowCorner = CCSprite::create("Ryzen_SquareShadow_001.png"_spr);
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
                        auto idInput = CCTextInputNode::create(290.f, 20.f, "", 16, "chatFont.fnt");//"developer.modname"
                        idInput->setAllowedChars(" !\"#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
                        idInput->setPosition({ -40.000f, 30.000f });
                        idInput->setID("idInput");
                        SearchLayer_Menu->addChild(idInput);
                        //inputbg
                        CCScale9Sprite* idInputBG = CCScale9Sprite::create("square02_001.png");
                        idInputBG->setContentSize({ idInput->getContentSize().width * 2 + 20, idInput->getContentSize().height * 2 + 15 });
                        idInputBG->setScale(0.5f);
                        idInputBG->setOpacity(60);
                        idInputBG->setPosition(idInput->getPosition());
                        SearchLayer_Menu->addChild(idInputBG, -1);
                        auto SearchLayerbtn1Sprite = ButtonSprite::create("Reload", 0, false, "goldFont.fnt", "GJ_button_05.png", 0, 0.6);
                        auto SearchLayerbtn1 = CCMenuItemSpriteExtra::create(SearchLayerbtn1Sprite, ret, menu_selector(RyzenLayer::onBtn));
                        SearchLayerbtn1->setID("Search");
                        SearchLayerbtn1->setPositionY(30.000f);
                        SearchLayerbtn1->setPositionX(158.000f);
                        SearchLayerbtn1->CCMenuItemSpriteExtra::setScale(0.900f);
                        SearchLayer_Menu->addChild(SearchLayerbtn1);
                    }
                }
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
        auto pRyzenLayer = RyzenLayer::create();
        scene->addChild(pRyzenLayer, 1, 2816);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
    };
    void openMeForPacks(cocos2d::CCObject* object) {
        auto scene = CCScene::create();
        auto pRyzenLayer = RyzenLayer::create();
        pRyzenLayer->sendBtnFunc("switch");
        scene->addChild(pRyzenLayer, 1, 2816);
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
