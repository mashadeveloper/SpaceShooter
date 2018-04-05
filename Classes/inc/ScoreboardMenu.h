#ifndef __SCOREBOARD_MENU_H__
#define __SCOREBOARD_MENU_H__

#include "cocos2d.h"

class ScoreboardMenu : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(ScoreboardMenu)
};

#endif // __SCOREBOARD_MENU_H__

