#pragma once
#include <cocos2d.h>
#include "mod_utils.hpp"
#include "SimpleIni.h"

using namespace cocos2d;
using namespace cocos2d::extension;
using namespace gd;
inline bool HideEverySprite;
inline bool HideEveryFrame;
inline std::string HideEveryFrameByName;
class ReplaceAllFramesByName {
public:
	inline static std::string by;
	inline static std::string to;
};
inline bool HideEveryLabel;
inline bool DontProcessFrames;
inline FLAlertLayer* popuptoreplace;

void CreateCocosHooks();

inline CCSprite* (__cdecl* CCSprite_create)(const char*);
CCSprite* CCSprite_create_H(const char* name);

inline CCSprite* (__cdecl* CCSprite_createWithSpriteFrameName)(const char*);
CCSprite* CCSprite_createWithSpriteFrameName_H(const char* name);

inline CCLabelBMFont* (__cdecl* CCLabelBMFont_create)(const char*, const char*);
CCLabelBMFont* CCLabelBMFont_create_H(const char* str, const char* fntFile);
