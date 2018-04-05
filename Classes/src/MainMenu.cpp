#include "MainMenu.h"
#include "SimpleAudioEngine.h"

#include "MainScene.h"
#include "ScoreboardMenu.h"

USING_NS_CC;

Scene* MainMenu::createScene()
{
    return MainMenu::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in MainScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MainMenu::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto playItem = MenuItemFont::create("Play game", CC_CALLBACK_1(MainMenu::switchToMainScene, this));
    playItem->setPosition(0, 40);
    auto scoreboardItem = MenuItemFont::create("Scoreboard", CC_CALLBACK_1(MainMenu::switchToMainScene, this));
    //scoreboardItem->setPosition(0, 10);
    auto exitItem = MenuItemFont::create("Exit game", CC_CALLBACK_1(MainMenu::menuCloseCallback, this));
    exitItem->setPosition(0, -40);
    auto main_menu = Menu::create(playItem, exitItem, scoreboardItem, NULL);
    main_menu->setPosition(origin.x + visibleSize.width/2,
                           origin.y + visibleSize.height/2);
    this->addChild(main_menu, 1);


    return true;
}


void MainMenu::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}

void MainMenu::switchToMainScene(Ref* pSender)
{
    auto main = MainScene::createScene();
    Director::getInstance()->replaceScene(main);
}

void MainMenu::switchToScoreboardScene(Ref *pSender)
{
    auto scoreboard = ScoreboardMenu::createScene();
    Director::getInstance()->replaceScene(scoreboard);
}
