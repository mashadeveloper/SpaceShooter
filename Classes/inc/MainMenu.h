#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "cocos2d.h"

class MainMenu : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    void switchToMainScene(Ref* pSender);
    void switchToScoreboardScene(Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(MainMenu)
};

#endif // __MAIN_MENU_H__
