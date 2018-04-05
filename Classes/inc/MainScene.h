#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

#include "PlayerShip.h"
#include "EnemyShip.h"

#include <vector>

class MainScene : public cocos2d::Scene
{
public:
    MainScene()
    {
        player = new PlayerShip();
    }

    ~MainScene()
    {
        delete player;
        enemies.clear();
    }

    static cocos2d::Scene* createScene();

    virtual bool init();

    void makeMoveBackground(float dt);
    void followMouse(float dt);
    void checkShipExitFromScreen(float dt);
    void laserMoveFinished(Node* sender);

    void addEnemy(float dt);
    void processEnemyActions(float dt);
    void checkEnemiesExitFromScreen(float dt);
    void checkEnemySpacing(float dt);
    void calculateEnemyRotation(float dt);
    void enemyShooting(float dt);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event *event);
    void onKeyHold(float dt);

    void onMouseMove(cocos2d::Event *event);    
    void onMouseDown(cocos2d::Event *event);
    void onMouseUp(cocos2d::Event *event);
    void onMouseHold(float dt);

    // implement the "static create()" method manually
    CREATE_FUNC(MainScene)

private:
    cocos2d::PhysicsWorld *sceneWorld;
    void SetPhysicsWorld( cocos2d::PhysicsWorld *world ) { sceneWorld = world; }

    bool onContactBegin(cocos2d::PhysicsContact& contact);
    EnemyShip* getEnemyShipByNode(Node* node);
    EnemyLaser *getEnemyLaserByNode(Node* node);

    cocos2d::Size visibleSize;

    cocos2d::Node* back;

    PlayerShip* player;
    cocos2d::Label* playerHP;
    std::vector<EnemyShip*> enemies;
    std::vector<EnemyLaser*> enemyLasers;

    std::vector<cocos2d::EventKeyboard::KeyCode> heldKeys;
    cocos2d::Vec2 mousePosition;
    bool mouseDown = false;
};

#endif // __MAIN_SCENE_H__
