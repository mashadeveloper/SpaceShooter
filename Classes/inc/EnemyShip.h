#ifndef __ENEMY_SHIP_H__
#define __ENEMY_SHIP_H__

#include "Ship.h"

#include <string>

class EnemyLaser
{
public:
    cocos2d::Sprite* sprite;
    float damage;
    cocos2d::Sprite *getSprite() const;
    void setSprite(cocos2d::Sprite *value);
    float getDamage() const;
    void setDamage(float value);
};

class EnemyShip : public Ship
{
public:

    bool isOnAction() const;
    void setOnAction(bool value);

protected:

private:
    bool onAction;
};

#endif // __ENEMY_SHIP_H__
